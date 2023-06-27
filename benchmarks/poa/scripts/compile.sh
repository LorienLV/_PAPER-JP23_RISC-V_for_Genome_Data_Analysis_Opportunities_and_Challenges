#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" arch='-fno-vectorize' \
        BUILD_DIR=build_EPI_CC MSA_SPOA_OMP=msa_spoa_omp_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" BUILD_DIR=build_gcc MSA_SPOA_OMP=msa_spoa_omp_gcc
    fi
    ;;
esac
