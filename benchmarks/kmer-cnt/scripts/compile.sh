#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" arch='-fno-vectorize' \
        EXE=kmer-cnt_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' EXE=kmer-cnt_gcc
    fi
    ;;
esac
