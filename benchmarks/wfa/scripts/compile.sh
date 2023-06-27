#!/bin/bash

case "$GENARCH_BENCH_CLUSTER" in
QEMUVM)
    if true; then # EPI_CC
        # Undefine __clang__ to disable #pragma clang loop vectorize(enable)
        # We only allow the auto-vectorization of loops marked with:
        # ifdef __EPI_RISCV
        #     #pragma clang loop vectorize(enable)
        # endif
        if true; then
            make CC="${EPI_CC}" CXX="${EPI_CPP}" \
            arch='-fno-vectorize -U__clang__' \
            FOLDER_BUILD=build_EPI_CC FOLDER_BIN=bin_EPI_CC
        else
            # Automatically remove unneeded vsetvli instructions from
            # affine_wavefront_align.c:affine_wavefronts_compute_next main loop
            make CC="${EPI_CC} -S" CXX="${EPI_CPP} -S" \
            arch='-fno-vectorize -U__clang__' \
            FOLDER_BUILD=build_EPI_CC FOLDER_BIN=bin_EPI_CC

            pushd build_EPI_CC
            sed -r -i '/^[ \t]*vsetvli[ \t]+zero/d' "./affine_wavefront_align.o"
            for file in *.o; do
                mv -- "$file" "${file%.o}.s"
                ${EPI_CC} -c "${file%.o}.s"
            done
            popd

            make CC="${EPI_CC}" CXX="${EPI_CPP}" \
            arch='-fno-vectorize -U__clang__' \
            FOLDER_BUILD=build_EPI_CC FOLDER_BIN=bin_EPI_CC
        fi
    fi
    ;;
*)
    if true; then # GCC
        make CC="gcc" CXX="g++" arch='-march=native' \
        FOLDER_BUILD=build_gcc FOLDER_BIN=bin_gcc
    fi
    ;;
esac
