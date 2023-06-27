#!/bin/bash

# TODO: bwa-mem2 does not use a build folder, so we need to execute make clean
# before compiling with a different compiler. Fix the makefile to use a build folder.

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        # TODO: TARGET_ARCH=RISCV
        make CC="${EPI_CC}" CXX="${EPI_CPP}" BUILDDIR=build_EPI_CC
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' BUILDDIR=build_gcc
    fi
    ;;
esac
