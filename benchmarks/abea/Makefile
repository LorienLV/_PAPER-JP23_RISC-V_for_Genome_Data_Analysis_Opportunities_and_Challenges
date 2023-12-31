HDF5 ?= install
HTS ?= install

ifeq ($(arch),sse41)
	ARCH_FLAGS=-msse4.1
else ifeq ($(arch),avx2)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-march=core-avx2 #-xCORE-AVX2
	else	
		ARCH_FLAGS=-mavx2
	endif
else ifeq ($(arch),avx512)
	ifeq ($(CXX), icpc)
		ARCH_FLAGS=-xCORE-AVX512
	else	
		ARCH_FLAGS=-mavx512bw
	endif
else ifeq ($(arch),native)
		ARCH_FLAGS=-march=native
else ifneq ($(arch),)
	## To provide a different architecture flag like -march=core-avx2.
		ARCH_FLAGS=$(arch)
endif

HTS_VERSION = 1.9
HDF5_VERSION = 1.10.4
HDF5_MAJOR_MINOR = `echo $(HDF5_VERSION) | sed -E 's/\.[0-9]+$$//'`

ifeq ($(HDF5), install)
    HDF5_LIB = $(BUILD_DIR)/lib/libhdf5.a
    HDF5_INC = -I$(BUILD_DIR)/include
    LDFLAGS += $(HDF5_LIB) -ldl
else
ifneq ($(HDF5), autoconf)
    HDF5_LIB =
    HDF5_SYS_LIB = `pkg-config --libs hdf5`
    HDF5_INC = `pkg-config --cflags-only-I hdf5`
endif
endif

ifeq ($(HTS), install)
    HTS_LIB = $(BUILD_DIR)/lib/libhts.a
    HTS_INC = -I$(BUILD_DIR)/include
    LDFLAGS += $(HTS_LIB)
else
ifneq ($(HTS), autoconf)
    HTS_LIB =
    HTS_SYS_LIB = `pkg-config --libs htslib`
    HTS_INC = `pkg-config --cflags-only-I htslib`
endif
endif

CPPFLAGS += $(HDF5_INC) $(HTS_INC)
LDFLAGS += $(HTS_SYS_LIB) $(HDF5_SYS_LIB)

CC       = gcc
CXX      = g++
LANG 	 = -x c++
CFLAGS   += -g -Wall -O2 -std=c++11 $(ARCH_FLAGS)
LDFLAGS  += $(LIBS) -lpthread -lz -rdynamic
BUILD_DIR = build

# Perf
PERF_ANALYSIS=0

# Intel VTune Profiler
VTUNE_ANALYSIS=0

ifeq ($(VTUNE_ANALYSIS),1)
	INCLUDES+=$(VTUNE_INCLUDES)
	LDFLAGS+=$(VTUNE_LDFLAGS)
endif

# Fujitsu Advanced Profiler
FAPP_ANALYSIS=0

# Dynamorio
DYNAMORIO_ANALYSIS=0

# PWR
PWR=0

ifeq ($(PWR),1)
	INCLUDES+=$(PWR_INCLUDES)
	LDFLAGS+=$(PWR_LDFLAGS)
endif

# RAPL_STOPWATCH
RAPL_STOPWATCH=0

ifeq ($(RAPL_STOPWATCH),1)
	INCLUDES+=$(RAPL_STOPWATCH_INCLUDES)
	LDFLAGS+=$(RAPL_STOPWATCH_LDFLAGS)
endif

# If input, processing and output are not interleaved (serial mode).
# Do not use pthread (to work with FAPP)
IO_PROC_NO_INTERLEAVE=1

CPPFLAGS+=-DPERF_ANALYSIS=$(PERF_ANALYSIS) -DVTUNE_ANALYSIS=$(VTUNE_ANALYSIS) -DFAPP_ANALYSIS=$(FAPP_ANALYSIS) -DDYNAMORIO_ANALYSIS=$(DYNAMORIO_ANALYSIS) -DPWR=$(PWR) -DRAPL_STOPWATCH=$(RAPL_STOPWATCH) -DIO_PROC_NO_INTERLEAVE=$(IO_PROC_NO_INTERLEAVE)

$(info $(shell mkdir -p $(BUILD_DIR)))

BINARY = f5c
OBJ = $(BUILD_DIR)/main.o \
      $(BUILD_DIR)/meth_main.o \
      $(BUILD_DIR)/f5c.o \
      $(BUILD_DIR)/events.o \
      $(BUILD_DIR)/nanopolish_read_db.o \
      $(BUILD_DIR)/nanopolish_index.o \
      $(BUILD_DIR)/nanopolish_fast5_io.o \
      $(BUILD_DIR)/model.o \
      $(BUILD_DIR)/align.o \
      $(BUILD_DIR)/meth.o \
      $(BUILD_DIR)/hmm.o \
      $(BUILD_DIR)/freq.o \
      $(BUILD_DIR)/eventalign.o \
      $(BUILD_DIR)/freq_merge.o

PREFIX = /usr/local
VERSION = `git describe --tags`

# CUDA_ROOT = /usr/local/cuda
# CUDA_LIB ?= $(CUDA_ROOT)/lib64
# CUDA_OBJ = $(BUILD_DIR)/f5c_cuda.o $(BUILD_DIR)/align_cuda.o
# NVCC ?= nvcc
# CUDA_CFLAGS += -g  -O3 -std=c++11 -lineinfo $(CUDA_ARCH) -Xcompiler -Wall
# CUDA_LDFLAGS = -L$(CUDA_LIB) -lcudart_static -lrt -ldl
# OBJ += $(BUILD_DIR)/gpucode.o $(CUDA_OBJ)
# CPPFLAGS += -DHAVE_CUDA=1

.PHONY: clean distclean

$(BINARY): src/config.h $(HTS_LIB) $(HDF5_LIB) $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) $(LDFLAGS) $(CUDA_LDFLAGS) -o $@

$(BUILD_DIR)/main.o: src/main.c src/f5cmisc.h src/error.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/meth_main.o: src/meth_main.c src/f5c.h src/fast5lite.h src/f5cmisc.h src/logsum.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/f5c.o: src/f5c.c src/f5c.h src/fast5lite.h src/f5cmisc.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/events.o: src/events.c src/f5c.h src/fast5lite.h src/f5cmisc.h src/fast5lite.h src/nanopolish_read_db.h src/ksort.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/nanopolish_read_db.o: src/nanopolish_read_db.c src/nanopolish_read_db.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/nanopolish_index.o: src/nanopolish_index.c src/nanopolish_read_db.h src/fast5lite.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/nanopolish_fast5_io.o: src/nanopolish_fast5_io.c src/fast5lite.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/model.o: src/model.c src/model.h src/f5c.h src/fast5lite.h src/f5cmisc.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/align.o: src/align.c src/f5c.h src/fast5lite.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/meth.o: src/meth.c src/f5c.h src/fast5lite.h src/f5cmisc.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/hmm.o: src/hmm.c src/f5c.h src/fast5lite.h src/f5cmisc.h src/matrix.h src/logsum.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/freq.o: src/freq.c src/khash.h
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/eventalign.o: src/eventalign.c
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

$(BUILD_DIR)/freq_merge.o: src/freq_merge.c
	$(CXX) $(INCLUDES) $(CFLAGS) $(CPPFLAGS) $(LANG) $< -c -o $@

# cuda stuff
# We do not have GPUs

# $(BUILD_DIR)/gpucode.o: $(CUDA_OBJ)
# 	$(NVCC) $(CUDA_CFLAGS) -dlink $^ -o $@

# $(BUILD_DIR)/f5c_cuda.o: src/f5c.cu src/error.h src/f5c.h src/fast5lite.h src/f5cmisc.cuh src/f5cmisc.h
# 	$(NVCC) -x cu $(CUDA_CFLAGS) $(CPPFLAGS) -rdc=true -c $< -o $@

# $(BUILD_DIR)/align_cuda.o: src/align.cu src/f5c.h src/fast5lite.h src/f5cmisc.cuh
# 	$(NVCC) -x cu $(CUDA_CFLAGS) $(CPPFLAGS) -rdc=true -c $< -o $@

src/config.h:
	echo "/* Default config.h generated by Makefile */" >> $@
	echo "#define HAVE_HDF5_H 1" >> $@

$(BUILD_DIR)/lib/libhts.a:
	cp htslib.tar.bz2 $(BUILD_DIR)
	tar -xf $(BUILD_DIR)/htslib.tar.bz2 -C $(BUILD_DIR)
	mv $(BUILD_DIR)/htslib-$(HTS_VERSION) $(BUILD_DIR)/htslib
	rm -f $(BUILD_DIR)/htslib.tar.bz2
	cd $(BUILD_DIR)/htslib && \
	CC='$(CC)' CXX='$(CXX)' \
	CFLAGS='-O2 $(ARCH_FLAGS)' CXXFLAGS='-O2 $(ARCH_FLAGS)' \
	./configure --prefix=`pwd`/../ --enable-bz2=no --enable-lzma=no --with-libdeflate=no --enable-libcurl=no  --enable-gcs=no --enable-s3=no && \
	$(MAKE) -j8 && \
	$(MAKE) install

$(BUILD_DIR)/lib/libhdf5.a:
	cp hdf5.tar.bz2 $(BUILD_DIR)
	tar -xf $(BUILD_DIR)/hdf5.tar.bz2 -C $(BUILD_DIR)
	mv $(BUILD_DIR)/hdf5-$(HDF5_VERSION) $(BUILD_DIR)/hdf5
	rm -f $(BUILD_DIR)/hdf5.tar.bz2
	cd $(BUILD_DIR)/hdf5 && \
	CC='$(CC)' CXX='$(CXX)' \
	CFLAGS='-O2 $(ARCH_FLAGS)' CXXFLAGS='-O2 $(ARCH_FLAGS)' \
	./configure --prefix=`pwd`/../ && \
	$(MAKE) -j8 && \
	$(MAKE) install

clean:
	rm -rf $(BINARY) $(BUILD_DIR)
