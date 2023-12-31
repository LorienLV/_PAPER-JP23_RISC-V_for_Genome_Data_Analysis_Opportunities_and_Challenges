#!/bin/bash

# The folder that contains the inputs.
inputs_path="$GENARCH_BENCH_INPUTS_ROOT/abea"

if [[ -z "$inputs_path" || ! -d "$inputs_path" ]]; then
    echo "ERROR: You have not set a valid input folder $inputs_path"
    exit 1
fi

scriptfolder="$(dirname $(realpath $0))"
binaries_path="$(dirname "$scriptfolder")"

# Clean the stage folder of the jobs after finishing? 1 -> yes, 0 -> no.
clean=1

# The name of the job.
job="ABEA-REGRESSION-LARGE"

# Commands to run.
# You can access the number of mpi-ranks using the environment variable
# MPI_RANKS and the number of omp-threads using the environment variable
# OMP_NUM_THREADS:
# commands=(
#    'command $MPI_RANKS $OMP_NUM_THREADS'
#)
# OR
# commands=(
#    "command \$MPI_RANKS \$OMP_NUM_THREADS"
#)

# Everything you want to do before executing the commands.
before_command="export OMP_PROC_BIND=true; export OMP_PLACES=cores;"

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    before_command+="${VEHAVE}"
    commands=(
        "$binaries_path/f5c_EPI_CC"
    )

    parallelism=(
        'nodes=1, mpi=1, omp=1'
        'nodes=1, mpi=1, omp=2'
    )
    ;;
*)
    commands=(
        "$binaries_path/f5c_gcc"
    )

    parallelism=(
        'nodes=1, mpi=1, omp=1'
        'nodes=1, mpi=1, omp=2'
    )
    ;;
esac

# Additional arguments to pass to the commands.
command_opts="eventalign -b "$inputs_path"/large/10000reads.bam \
-g "$inputs_path"/humangenome.fa -r "$inputs_path"/10000reads.fastq -B 3.7M \
-t \$OMP_NUM_THREADS > events.tsv"

#
# This function is executed before launching a job. You can use this function to
# prepare the stage folder of the job.
#
before_run() (
    job_name="$1"
)

#
# This function is executed when a job has finished. You can use this function to
# perform a sanity check and to output something in the report of a job.
#
# echo: The message that you want to output in the report of the job.
# return: 0 if the run is correct, 1 otherwise.
#
after_run() (
    job_name="$1"

    wall_time="$(tac "$job_name.err" | grep -m 1 "Data processing time:" | cut -d ' ' -f 5)"

    echo "Data processing time: $wall_time s"

    # Check that columns "reference_kmer" and "model_kmer" are identical in the
    # reference and the output files. We allow some diffs since the output depends
    # on floating-point operations.
    ndiffs="$(diff --speed-large-files \
              <(awk '{print $3$10}' "events.tsv") \
              <(awk '{print $3$10}' "$inputs_path/large-reference.tsv") \
              | grep "^>" | wc -l)"
    if [[ ! -s "events.tsv" || $ndiffs -gt 50 ]]; then
        echo "The output file is not identical to the reference file"
        return 1 # Failure
    fi

    cat "$job_name.err" | grep "Energy consumption:"

    return 0 # OK
)

source "$scriptfolder/../../run_wrapper.sh"
