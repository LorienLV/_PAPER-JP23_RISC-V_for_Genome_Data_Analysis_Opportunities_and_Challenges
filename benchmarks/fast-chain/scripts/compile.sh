#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" arch='-fno-vectorize' \
        BUILD_PATH=build_EPI_CC BIN_NAME=chain_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' \
        BUILD_PATH=build_gcc BIN_NAME=chain_gcc
    fi
    ;;
esac
