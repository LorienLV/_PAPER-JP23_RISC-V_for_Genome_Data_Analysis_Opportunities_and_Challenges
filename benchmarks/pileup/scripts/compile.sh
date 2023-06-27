#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" BUILD_DIR=build_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' BUILD_DIR=build_gcc
    fi
    ;;
esac
