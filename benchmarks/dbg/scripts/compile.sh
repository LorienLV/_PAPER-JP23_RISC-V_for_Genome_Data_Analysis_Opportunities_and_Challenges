#!/bin/bash

#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    # BEFORE COMPILING: sudo yum install libcurl-devel
    if true; then # EPI_CC
        make CC="${EPI_CC}" CXX="${EPI_CPP}" arch='-fno-vectorize' \
        BUILD_DIR=build_EPI_CPP
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' BUILD_DIR=build_gcc
    fi
    ;;
esac
