/**
 * @author Lorién López Villellas (lorien.lopez@bsc.es)
 *
 * @brief Compute the revered vector of a given vector.
 *
 * for element = 0 to gvl - 1
 *     result[element] = a[vl - 1 - element]
 *
 * result[gvl : VLMAX] = ? (undefined)
 *
 *
 */

#ifndef EPI_SOFTW_VREV_H
#define EPI_SOFTW_VREV_H

#include <stdint.h>

// #define DEBUG 1 // For debugging, uncomment DEBUG and change the extension of the file to .c
#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

/**
 * EPI 8-bits vector reverse.
 *
 * @param src Source 8 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The source vector reversed.
 */
#ifndef DEBUG
inline
#endif
__epi_8xi8 __builtin_epi_softw_vrev_8xi8(__epi_8xi8 src, unsigned long int vl) {
    // vidx = 0, 1 ... vl-1
    __epi_8xi8 vidx = __builtin_epi_vid_8xi8(vl);
    // vidx_rev = vl-1, vl-2 ... 0
    __epi_8xi8 vvl_m1 = __builtin_epi_vbroadcast_8xi8(vl - 1, vl);
    __epi_8xi8 vidx_rev = __builtin_epi_vsub_8xi8(vvl_m1, vidx, vl);

    // src_rev = src[vl-1], src[vl-2] ... src[0]
    __epi_8xi8 src_rev = __builtin_epi_vrgather_8xi8(src, vidx_rev, vl);

    return src_rev;
}

/**
 * EPI 16-bits vector reverse.
 *
 * @param src Source 16 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The source vector reversed.
 */
#ifndef DEBUG
inline
#endif
__epi_4xi16 __builtin_epi_softw_vrev_4xi16(__epi_4xi16 src, unsigned long int vl) {
    // vidx = 0, 1 ... vl-1
    __epi_4xi16 vidx = __builtin_epi_vid_4xi16(vl);
    // vidx_rev = vl-1, vl-2 ... 0
    __epi_4xi16 vvl = __builtin_epi_vbroadcast_4xi16(vl - 1, vl);
    __epi_4xi16 vidx_rev = __builtin_epi_vsub_4xi16(vvl, vidx, vl);

    // src_rev = src[vl-1], src[vl-2] ... src[0]
    __epi_4xi16 src_rev = __builtin_epi_vrgather_4xi16(src, vidx_rev, vl);

    return src_rev;
}

/**
 * EPI 32-bits vector reverse.
 *
 * @param src Source 32 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The source vector reversed.
 */
#ifndef DEBUG
inline
#endif
__epi_2xi32 __builtin_epi_softw_vrev_2xi32(__epi_2xi32 src, unsigned long int vl) {
    // vidx = 0, 1 ... vl-1
    __epi_2xi32 vidx = __builtin_epi_vid_2xi32(vl);
    // vidx_rev = vl-1, vl-2 ... 0
    __epi_2xi32 vvl = __builtin_epi_vbroadcast_2xi32(vl - 1, vl);
    __epi_2xi32 vidx_rev = __builtin_epi_vsub_2xi32(vvl, vidx, vl);

    // src_rev = src[vl-1], src[vl-2] ... src[0]
    __epi_2xi32 src_rev = __builtin_epi_vrgather_2xi32(src, vidx_rev, vl);

    return src_rev;
}

/**
 * EPI 64-bits vector reverse.
 *
 * @param src Source 64 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The source vector reversed.
 */
#ifndef DEBUG
inline
#endif
__epi_1xi64 __builtin_epi_softw_vrev_1xi64(__epi_1xi64 src, unsigned long int vl) {
    // vidx = 0, 1 ... vl-1
    __epi_1xi64 vidx = __builtin_epi_vid_1xi64(vl);
    // vidx_rev = vl-1, vl-2 ... 0
    __epi_1xi64 vvl = __builtin_epi_vbroadcast_1xi64(vl - 1, vl);
    __epi_1xi64 vidx_rev = __builtin_epi_vsub_1xi64(vvl, vidx, vl);

    // src_rev = src[vl-1], src[vl-2] ... src[0]
    __epi_1xi64 src_rev = __builtin_epi_vrgather_1xi64(src, vidx_rev, vl);

    return src_rev;
}

#ifdef DEBUG
char test_8b() {
    unsigned int n = 255;

    int8_t *src = malloc(n * sizeof(src[0]));
    int8_t *src_rev = malloc(n * sizeof(src_rev[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e8, __epi_m1);

        __epi_8xi8 srci = __builtin_epi_vload_8xi8(src + i, vl);

        __epi_8xi8 srci_rev = __builtin_epi_softw_vrev_8xi8(srci, vl);

        __builtin_epi_vstore_8xi8(src_rev + i, srci_rev, vl);

        for (unsigned int j = 0; j < vl; j++) {
            if (src_rev[i + j] != src[i + vl - j - 1]) {
                return 1;
            }
        }
    }

    free(src);
    free(src_rev);

    return 0;
}

char test_16b() {
    unsigned int n = 255;

    int16_t *src = malloc(n * sizeof(src[0]));
    int16_t *src_rev = malloc(n * sizeof(src_rev[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e16, __epi_m1);

        __epi_4xi16 srci = __builtin_epi_vload_4xi16(src + i, vl);

        __epi_4xi16 srci_rev = __builtin_epi_softw_vrev_4xi16(srci, vl);

        __builtin_epi_vstore_4xi16(src_rev + i, srci_rev, vl);

        for (unsigned int j = 0; j < vl; j++) {
            if (src_rev[i + j] != src[i + vl - j - 1]) {
                return 1;
            }
        }
    }

    free(src);
    free(src_rev);

    return 0;
}

char test_32b() {
    unsigned int n = 255;

    int32_t *src = malloc(n * sizeof(src[0]));
    int32_t *src_rev = malloc(n * sizeof(src_rev[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e32, __epi_m1);

        __epi_2xi32 srci = __builtin_epi_vload_2xi32(src + i, vl);

        __epi_2xi32 srci_rev = __builtin_epi_softw_vrev_2xi32(srci, vl);

        __builtin_epi_vstore_2xi32(src_rev + i, srci_rev, vl);

        for (unsigned int j = 0; j < vl; j++) {
            if (src_rev[i + j] != src[i + vl - j - 1]) {
                return 1;
            }
        }
    }

    free(src);
    free(src_rev);

    return 0;
}

char test_64b() {
    unsigned int n = 255;

    int64_t *src = malloc(n * sizeof(src[0]));
    int64_t *src_rev = malloc(n * sizeof(src_rev[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e64, __epi_m1);

        __epi_1xi64 srci = __builtin_epi_vload_1xi64(src + i, vl);

        __epi_1xi64 srci_rev = __builtin_epi_softw_vrev_1xi64(srci, vl);

        __builtin_epi_vstore_1xi64(src_rev + i, srci_rev, vl);

        for (unsigned int j = 0; j < vl; j++) {
            if (src_rev[i + j] != src[i + vl - j - 1]) {
                return 1;
            }
        }
    }

    free(src);
    free(src_rev);

    return 0;
}

int main(int argc, char const *argv[]) {
#if 1
    char error8 = test_8b();
    if (error8) {
        printf("test_8b ERROR\n");
    }
    else {
        printf("test_8b PASSED\n");
    }
#endif

#if 1
    char error16 = test_16b();
    if (error16) {
        printf("test_16b ERROR\n");
    }
    else {
        printf("test_16b PASSED\n");
    }
#endif

#if 1
    char error32 = test_32b();
    if (error32) {
        printf("test_32b ERROR\n");
    }
    else {
        printf("test_32b PASSED\n");
    }
#endif

#if 1
    char error64 = test_64b();
    if (error64) {
        printf("test_64b ERROR\n");
    }
    else {
        printf("test_64b PASSED\n");
    }
#endif

    return 0;
}
#endif

#endif
