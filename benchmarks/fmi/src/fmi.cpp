/*************************************************************************************
                           The MIT License

   BWA-MEM2  (Sequence alignment using Burrows-Wheeler Transform),
   Copyright (C) 2019  Vasimuddin Md, Sanchit Misra, Intel Corporation, Heng Li.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

Authors: Vasimuddin Md <vasimuddin.md@intel.com>; Sanchit Misra <sanchit.misra@intel.com>.
*****************************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <omp.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <fcntl.h>

#include "bwa.h"
#include "FMI_search.h"

#ifdef ENABLE_PARSEC_HOOKS
#include "hooks.h"
#endif

#if RAPL_STOPWATCH
	#include <rapl_stopwatch.h>
#endif
#if PWR
	#include <pwr.h>
#endif
#if VTUNE_ANALYSIS
    #include <ittnotify.h>
#endif
#if FAPP_ANALYSIS
    #include <fj_tool/fapp.h>
#endif
#if DYNAMORIO_ANALYSIS
    #if defined(__x86_64__) || defined(_M_X64)
        #define __DR_START_TRACE() { asm volatile ("nopw 0x24"); }
        #define __DR_STOP_TRACE() { asm volatile ("nopw 0x42"); }
    #else
        #error invalid TARGET
    #endif
#endif

#define PRINT_OUTPUT 1

#define CLMUL (ALIGNMENT / 8) // 8 x 64 bit cache line
// #define QUERY_DB_SIZE 1280000000
#define QUERY_DB_SIZE 2560000000L
int myrank, num_ranks;

int main(int argc, char **argv) {
    if(argc!=6)
    {
        printf("Need five arguments : ref_file query_set batch_size minSeedLen n_threads\n");
        return 1;
    }

    int32_t numReads = 0;
    int64_t total_size = 0;
    gzFile fp = gzopen(argv[2], "r");
	if (fp == 0)
	{
		fprintf(stderr, "[E::%s] fail to open file `%s'.\n", __func__, argv[2]);
        exit(EXIT_FAILURE);
	}
    
    printf("before reading sequences\n");
    std::chrono::steady_clock::time_point begin_reading = std::chrono::steady_clock::now();

    bseq1_t *seqs = bseq_read_one_fasta_file(QUERY_DB_SIZE, &numReads, fp, &total_size);

    if(seqs == NULL)
    {
        printf("ERROR! seqs = NULL\n");
        exit(EXIT_FAILURE);
    }
    int32_t *query_cum_len_ar = (int32_t *)_mm_malloc(numReads * sizeof(int32_t), ALIGNMENT);

    FMI_search *fmiSearch = new FMI_search(argv[1]);
    fmiSearch->load_index();
    
    std::chrono::steady_clock::time_point end_reading = std::chrono::steady_clock::now();

    int max_readlength = seqs[0].l_seq;
    int min_readlength = seqs[0].l_seq;
    for(int i = 1; i < numReads; i++)
    {
        if(max_readlength < seqs[i].l_seq)
            max_readlength = seqs[i].l_seq;
        if(min_readlength > seqs[i].l_seq)
            min_readlength = seqs[i].l_seq;
    }
    assert(max_readlength > 0);
    assert(max_readlength < 10000);
    assert(numReads > 0);
    assert(numReads * max_readlength < QUERY_DB_SIZE);
    printf("numReads = %d, max_readlength = %d, min_readlength = %d\n", numReads, max_readlength, min_readlength);
    uint8_t *enc_qdb=(uint8_t *)malloc((int64_t)numReads * max_readlength * sizeof(uint8_t));

    int64_t cind,st;
#if 0
    printf("Priting query\n");
    for(st = 0; st < max_readlength; st++)
    {
        printf("%c", seqs[0].seq[st]);
    }
    printf("\n");
#endif
    uint64_t r;
    for (st=0; st < numReads; st++) {
        query_cum_len_ar[st] = st * max_readlength;
        cind=st*max_readlength;
        for(r = 0; r < max_readlength; ++r) {
            switch(seqs[st].seq[r])
            {
                case 'A': enc_qdb[r+cind]=0;
                          break;
                case 'C': enc_qdb[r+cind]=1;
                          break;
                case 'G': enc_qdb[r+cind]=2;
                          break;
                case 'T': enc_qdb[r+cind]=3;
                          break;
                default: enc_qdb[r+cind]=4;
            }
            //printf("%c %d\n", seqs[st].seq[r], enc_qdb[r + cind]);
        }
    }

    int batch_size=0;
    batch_size=atoi(argv[3]);
    assert(batch_size > 0);
    assert(batch_size <= numReads);


    int32_t minSeedLen = atoi(argv[4]);
    int numthreads=atoi(argv[5]);

    const int splitWidth = 10;
    const int maxMemIntv = 20;
    const double splitFactor = 1.5;

    assert(numthreads > 0);
    assert(numthreads <= omp_get_max_threads());
    SMEM *matchArray[CLMUL * numthreads];
    int32_t *min_intv_array[CLMUL * numthreads];
    int32_t *rid_array[CLMUL * numthreads];
    int16_t *query_pos_array[CLMUL * numthreads];

    int64_t num_batches = (numReads + batch_size - 1 ) / batch_size;
    int64_t *numTotalSmem = (int64_t *)_mm_malloc(num_batches * sizeof(int64_t), ALIGNMENT);

    int *batchTid = (int *)_mm_malloc(num_batches * sizeof(*batchTid), ALIGNMENT);
    int64_t *batchStart = (int64_t *)_mm_malloc(num_batches * sizeof(*batchStart), ALIGNMENT);

    // uint64_t tim = __rdtsc();
    // sleep(1);
    // uint64_t proc_freq = __rdtsc() - tim;

#pragma omp parallel num_threads(numthreads)
    {
        int tid = omp_get_thread_num();

        if(tid == 0)
            printf("Running %d threads\n", omp_get_num_threads());
    }

#if RAPL_STOPWATCH
	int err = rapl_stopwatch_api_init();
	if (err) {
		fprintf(stderr, "Error initializing the RAPL-stopwatch API\n");
	}

	rapl_stopwatch_t rapl_sw;
	rapl_stopwatch_init(&rapl_sw);

	rapl_stopwatch_play(&rapl_sw);
#endif
#if PWR
	// 1. Initialize Power API
	PWR_Cntxt pwr_cntxt = NULL;
	PWR_CntxtInit(PWR_CNTXT_FX1000, PWR_ROLE_APP, "app", &pwr_cntxt);
	// 2. Get Object (In this step, get an Object that indicates the entire compute node.)
	PWR_Obj pwr_obj = NULL;
	PWR_CntxtGetObjByName(pwr_cntxt, "plat.node", &pwr_obj);
	// 3. Get electric energy at the start.
	double energy0 = 0.0;
	PWR_ObjAttrGetValue(pwr_obj, PWR_ATTR_MEASURED_ENERGY, &energy0, NULL);
#endif
#if PERF_ANALYSIS
	const char *perf_pipe = "perf_ctl.fifo";
    const char *perf_enable = "enable";
    const char *perf_disable = "disable";

    int perf_pipe_fd = open(perf_pipe, O_WRONLY);
    if (perf_pipe_fd == -1) {
        fprintf(stderr, "ERROR opening the Perf pipe\n");
    }
    write(perf_pipe_fd, perf_enable, strlen(perf_enable));
#endif
#if VTUNE_ANALYSIS
    __itt_resume();
#endif
#if FAPP_ANALYSIS
    fapp_start("computing", 1, 0);
#endif
#if DYNAMORIO_ANALYSIS
    __DR_START_TRACE();
#endif

    int64_t i;
    int64_t startTick, endTick;
    // startTick = __rdtsc();

    std::chrono::steady_clock::time_point begin_computing = std::chrono::steady_clock::now();

    memset(numTotalSmem, 0, num_batches * sizeof(int64_t));

    memset(batchTid, 0, num_batches * sizeof(*batchTid));
    memset(batchStart, 0, num_batches * sizeof(*batchStart));

    //int64_t workTicks[CLMUL * numthreads];
    //memset(workTicks, 0, CLMUL * numthreads * sizeof(int64_t));
    int64_t perThreadQuota = numReads / numthreads;

    int split_len = (int)(minSeedLen * splitFactor + .499);

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_begin();
#endif

#pragma omp parallel num_threads(numthreads)
    {
        int32_t tid = omp_get_thread_num();
        int64_t matchArrayAlloc = perThreadQuota * 20;
        matchArray[CLMUL * tid] = (SMEM *)malloc(matchArrayAlloc * sizeof(SMEM));
        min_intv_array[CLMUL * tid] = (int32_t *)malloc(matchArrayAlloc * sizeof(int32_t));
        rid_array[CLMUL * tid] = (int32_t *)malloc(matchArrayAlloc * sizeof(int32_t));
        query_pos_array[CLMUL * tid] = (int16_t *)malloc(matchArrayAlloc * sizeof(int16_t));

        int64_t myTotalSmems = 0;
        // int64_t startTick = __rdtsc();

#pragma omp for schedule(dynamic, 1)
        for(i = 0; i < numReads; i += batch_size)
        {
            // int64_t st1 = __rdtsc();
            int32_t batch_count = batch_size;
            if((i + batch_count) > numReads) batch_count = numReads - i;
            int32_t j;
            for(j = 0; j < batch_count; j++)
            {
                min_intv_array[CLMUL * tid][j] = 1;
                rid_array[CLMUL * tid][j] = j;
            }
            int32_t batch_id = i/batch_size;
            //printf("%d] i = %d, batch_count = %d, batch_size = %d\n", tid, i, batch_count, batch_size);
            //fflush(stdout);
            if((matchArrayAlloc - myTotalSmems) < (batch_size * max_readlength))
            {
                printf("%d] realloc\n", tid);
                fflush(stdout);
                matchArrayAlloc *= 2;
                matchArray[CLMUL * tid] = (SMEM *)realloc(matchArray[CLMUL * tid], matchArrayAlloc * sizeof(SMEM)); 
                min_intv_array[CLMUL * tid] = (int32_t *)realloc(min_intv_array[CLMUL * tid], matchArrayAlloc * sizeof(int32_t)); 
                rid_array[CLMUL * tid] = (int32_t *)realloc(rid_array[CLMUL * tid], matchArrayAlloc * sizeof(int32_t));
                query_pos_array[CLMUL * tid] = (int16_t *)realloc(query_pos_array[CLMUL * tid], matchArrayAlloc * sizeof(int16_t));
            }
            int64_t num_smem1 = 0, num_smem2 = 0, num_smem3 = 0;
            fmiSearch->getSMEMsAllPosOneThread(enc_qdb + i * max_readlength,
                    min_intv_array[CLMUL * tid],
                    rid_array[CLMUL * tid],
                    batch_count,
                    batch_size,
                    seqs + i,
                    query_cum_len_ar,
                    max_readlength,
                    minSeedLen,
                    &matchArray[CLMUL * tid][myTotalSmems],
                    &num_smem1);
            
            int64_t pos = 0;
            for (j = 0; j < num_smem1; j++) {
                SMEM *p = &matchArray[CLMUL * tid][myTotalSmems + j];
                int start = p->m, end = p->n +1;
                if (end - start < split_len || p->s > splitWidth) continue;
                // printf("%d:%d:%ld\n", start, end, p->s);
                rid_array[CLMUL * tid][pos] = p->rid;
                query_pos_array[CLMUL * tid][pos] = (end + start)>>1;
                min_intv_array[CLMUL * tid][pos] = p->s + 1;
                pos++;
            }
            
            // Reseed
            fmiSearch->getSMEMsOnePosOneThread(enc_qdb + i * max_readlength,
                    query_pos_array[CLMUL * tid],
                    min_intv_array[CLMUL * tid],
                    rid_array[CLMUL * tid],
                    pos,
                    pos,
                    seqs + i,
                    query_cum_len_ar,
                    max_readlength,
                    minSeedLen,
                    &matchArray[CLMUL * tid][myTotalSmems + num_smem1],
                    &num_smem2);
            // LAST
            for(j = 0; j < batch_count; j++)
            {
                min_intv_array[CLMUL * tid][j] = maxMemIntv;
            }
            num_smem3 = fmiSearch->bwtSeedStrategyAllPosOneThread(enc_qdb + i * max_readlength,
                    min_intv_array[CLMUL * tid],
                    batch_count,
                    seqs + i,
                    query_cum_len_ar,
                    minSeedLen + 1,
                    &matchArray[CLMUL * tid][myTotalSmems + num_smem1 + num_smem2]);
            int64_t totalSmem = num_smem1 + num_smem2 + num_smem3; 
            numTotalSmem[batch_id] = totalSmem;
            batchTid[batch_id] = tid;
            batchStart[batch_id] = myTotalSmems;
            for(j = 0; j < totalSmem; j++)
            {
                matchArray[CLMUL * tid][myTotalSmems + j].rid += i;
            }
            fmiSearch->sortSMEMs(matchArray[CLMUL * tid] + myTotalSmems,
                    numTotalSmem + batch_id,
                    batch_count,
                    max_readlength,
                    1);
            myTotalSmems += totalSmem; 
            // int64_t et1 = __rdtsc();
            //workTicks[CLMUL * tid] += (et1 - st1);
        }

        // int64_t endTick = __rdtsc();
        //printf("%d] %ld ticks, workTicks = %ld\n", tid, endTick - startTick, workTicks[CLMUL * tid]);
    }

#ifdef ENABLE_PARSEC_HOOKS
    __parsec_roi_end();
#endif

    std::chrono::steady_clock::time_point end_computing = std::chrono::steady_clock::now();

    // endTick = __rdtsc();

#if DYNAMORIO_ANALYSIS
    __DR_STOP_TRACE();
#endif
#if PERF_ANALYSIS
    write(perf_pipe_fd, perf_disable, strlen(perf_disable));
    close(perf_pipe_fd);
#endif
#if VTUNE_ANALYSIS
    __itt_pause();
#endif
#if FAPP_ANALYSIS
    fapp_stop("computing", 1, 0);
#endif
#if PWR
	// 3. Get electric energy at the end.
	double energy1 = 0.0;
	PWR_ObjAttrGetValue(pwr_obj, PWR_ATTR_MEASURED_ENERGY, &energy1, NULL);
	// 4. Terminate processing of Power API
	PWR_CntxtDestroy(pwr_cntxt);

	fprintf(stderr, "Energy consumption: %0.4lf J\n", energy1 - energy0);
#endif
#if RAPL_STOPWATCH
	rapl_stopwatch_pause(&rapl_sw);

	uint64_t count = 0;
	err = rapl_stopwatch_get_mj(&rapl_sw, RAPL_NODE, &count);
	if (err) {
		fprintf(stderr, "Error reading the RAPL-stopwatch counter\n");
	}

	fprintf(stderr, "Energy consumption: %0.4lf J\n", (double)count / 1E3);

	rapl_stopwatch_destroy(&rapl_sw);
	rapl_stopwatch_api_destroy();
#endif

    //int64_t sumTicks = 0;
    //int64_t maxTicks = 0;
    //for(i = 0; i < numthreads; i++)
    //{
    //    sumTicks += workTicks[CLMUL * i];
    //    if(workTicks[CLMUL * i] > maxTicks) maxTicks = workTicks[CLMUL * i];
    //}
    //double avgTicks = (sumTicks * 1.0) / numthreads;
    //printf("avgTicks = %lf, maxTicks = %ld, load imbalance = %lf\n", avgTicks, maxTicks, maxTicks/avgTicks);

    //printf("Consumed: %ld cycles, %0.4lf sec\n", endTick - startTick, (endTick - startTick) * 1.0 / proc_freq);

    int64_t totalSmem = 0;
    int32_t batch_id = 0;
    for(batch_id = 0; batch_id < num_batches; batch_id++)
    {
        totalSmem += numTotalSmem[batch_id];
    }
    std::cout << "totalSmems = " << totalSmem << "\n";
    std::cout << "Reading time: " <<
        std::chrono::duration<double>(end_reading - begin_reading).count()
        << " s\n";
    std::cout << "Computing time: " <<
        std::chrono::duration<double>(end_computing - begin_computing).count()
        << " s\n";

#ifdef PRINT_OUTPUT
    int32_t prevRid = -1;
    for(batch_id = 0; batch_id < num_batches; batch_id++)
    {
        int tid = batchTid[batch_id];
        int64_t start = batchStart[batch_id];
        SMEM *myMatchArray = matchArray[CLMUL * tid] + start;
        int64_t i;
        for(i = 0; i < numTotalSmem[batch_id]; i++)
        {
            SMEM smem = myMatchArray[i];
            if(smem.rid != prevRid)
            {
                int32_t j;
                for(j = prevRid + 1; j <= smem.rid; j++)
                    printf("%u:\n", j);
            }
            prevRid = smem.rid;
            printf("[%u,%u]", smem.m, smem.n + 1);
            // printf("%u, %u]", smem.k, smem.s);
#if 0
            printf(" ["); 
            int64_t u1, u2, u3;
            u1 = smem.k;
            u2 = smem.k + smem.s;
            for(u3 = u1; u3 < u2; u3++)
            {
                printf("%ld,", fmiSearch->get_sa_entry(u3));
            }
            printf("]"); 
#endif
            printf("\n");
        }
    }
#endif
    _mm_free(query_cum_len_ar);
    free(enc_qdb);
    for(int tid = 0; tid < numthreads; tid++)
    {
        free(matchArray[CLMUL * tid]);
        free(min_intv_array[CLMUL * tid]);
        free(query_pos_array[CLMUL * tid]);
        free(rid_array[CLMUL * tid]);
    }
    _mm_free(numTotalSmem);
    _mm_free(batchTid);
    _mm_free(batchStart);
    delete fmiSearch;
    return 0;
}

