#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" arch='-fno-vectorize' \
        FOLDER_BUILD=build_EPI_CC FOLDER_BIN=bin_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' \
        FOLDER_BUILD=build_gcc FOLDER_BIN=bin_gcc
    fi
    ;;
esac
