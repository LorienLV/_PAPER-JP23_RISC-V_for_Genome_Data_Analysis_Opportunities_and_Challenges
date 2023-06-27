/**
 * @author Lorién López Villellas (lorien.lopez@bsc.es)
 *
 * @brief 8, 16, 32 and 64 bits vector count leading zeros functions.
 *
 */

#ifndef EPI_SOFTW_VCLZ_H
#define EPI_SOFTW_VCLZ_H

#include <stdint.h>

// #define DEBUG 1 // For debugging, uncomment DEBUG and change the extension of the file to .c
#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

/**
 * EPI 8-bits vector count leading zeros.
 *
 * @param src Source 8 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of leading zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_8xi8 __builtin_epi_softw_vclz_8xi8(__epi_8xi8 src, unsigned long int vl) {
    // dest = 1 if src >= inm
    __epi_8xi1 ge_2to0 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00000000, vl), vl);
    __epi_8xi1 ge_2to1 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00000001, vl), vl);
    __epi_8xi1 ge_2to2 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00000011, vl), vl);
    __epi_8xi1 ge_2to3 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00000111, vl), vl);
    __epi_8xi1 ge_2to4 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00001111, vl), vl);
    __epi_8xi1 ge_2to5 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00011111, vl), vl);
    __epi_8xi1 ge_2to6 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b00111111, vl), vl);
    __epi_8xi1 ge_2to7 = __builtin_epi_vmsgtu_8xi8(src, __builtin_epi_vbroadcast_8xi8(0b01111111, vl), vl);

    // lz = ge_2toX ? Y : lz
    __epi_8xi8 lz = __builtin_epi_vbroadcast_8xi8(8, vl); // All elements in the vector are 8
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(7, vl), ge_2to0, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(6, vl), ge_2to1, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(5, vl), ge_2to2, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(4, vl), ge_2to3, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(3, vl), ge_2to4, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(2, vl), ge_2to5, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(1, vl), ge_2to6, vl);
    lz = __builtin_epi_vmerge_8xi8(lz, __builtin_epi_vbroadcast_8xi8(0, vl), ge_2to7, vl);

    return lz;
}

/**
 * EPI 16-bits vector count leading zeros.
 *
 * @param src Source 16 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of leading zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_4xi16 __builtin_epi_softw_vclz_4xi16(__epi_4xi16 src, unsigned long int vl) {
    // 8-bit count-leading-zeros. Cast result to 16 bits.
    __epi_4xi16 lz = (__epi_4xi16)__builtin_epi_softw_vclz_8xi8((__epi_8xi8)src, vl * 2);

    // atleast8lz = true if lz[hi] > 7 (true if 8 or more leading zeros)
    __epi_4xi1 atleast8lz = __builtin_epi_vmsgtu_4xi16(lz, __builtin_epi_vbroadcast_4xi16((1 << (8 + 3)) - 1, vl), vl);

    // Extract the highest 8 bits of each 16-bit element.
    __epi_4xi16 hipart = __builtin_epi_vsra_4xi16(lz, __builtin_epi_vbroadcast_4xi16(8, vl), vl); // dest >> 8

    // Extract the lowest 8 bits of each 16-bit element.
    __epi_4xi16 lowpart = __builtin_epi_vand_4xi16(lz, __builtin_epi_vbroadcast_4xi16((1 << 8) - 1, vl), vl); // lz & 0b11111111
    // If we take the low part, there are 8 or more leading zeros, 
    // then we need to add 8 to the low part.
    lowpart = __builtin_epi_vadd_4xi16(lowpart, __builtin_epi_vbroadcast_4xi16(8, vl), vl);

    //  dest = atleast8lz ? lowpart + 8 : hipart
    lz = __builtin_epi_vmerge_4xi16(hipart, lowpart, atleast8lz, vl);

    return lz;
}

/**
 * EPI 32-bits vector count leading zeros.
 *
 * @param src Source 32 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of leading zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_2xi32 __builtin_epi_softw_vclz_2xi32(__epi_2xi32 src, unsigned long int vl) {
    // 16-bit count-leading-zeros. Cast result to 32 bits.
    __epi_2xi32 lz = (__epi_2xi32)__builtin_epi_softw_vclz_4xi16((__epi_4xi16)src, vl * 2);

    // atleast16lz = true if lz[hi] > 15 (true if 16 or more leading zeros)
    __epi_2xi1 atleast16lz = __builtin_epi_vmsgtu_2xi32(lz, __builtin_epi_vbroadcast_2xi32((1 << (16 + 4)) - 1, vl), vl);

    // Extract the highest 16 bits of each 32-bit element.
    __epi_2xi32 hipart = __builtin_epi_vsra_2xi32(lz, __builtin_epi_vbroadcast_2xi32(16, vl), vl);

    // Extract the lowest 16 bits of each 32-bit element.
    __epi_2xi32 lowpart = __builtin_epi_vand_2xi32(lz, __builtin_epi_vbroadcast_2xi32((1 << 16) - 1, vl), vl);
    // If we take the low part, there are 16 or more leading zeros, 
    // then we need to add 16 to the low part.
    lowpart = __builtin_epi_vadd_2xi32(lowpart, __builtin_epi_vbroadcast_2xi32(16, vl), vl);

    //  dest = atleast16lz ? lowpart + 16 : hipart
    lz = __builtin_epi_vmerge_2xi32(hipart, lowpart, atleast16lz, vl);

    return lz;
}

/**
 * EPI 64-bits vector count leading zeros.
 *
 * @param src Source 64 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of leading zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_1xi64 __builtin_epi_softw_vclz_1xi64(__epi_1xi64 src, unsigned long int vl) {
    // 32-bit count-leading-zeros. Cast result to 64 bits.
    __epi_1xi64 lz = (__epi_1xi64)__builtin_epi_softw_vclz_2xi32((__epi_2xi32)src, vl * 2);

    // atleast32lz = true if lz[hi] > 31 (true if 32 or more leading zeros)
    __epi_1xi1 atleast32lz = __builtin_epi_vmsgtu_1xi64(lz, __builtin_epi_vbroadcast_1xi64(((uint64_t)1 << (32 + 5)) - 1, vl), vl);

    // Extract the highest 32 bits of each 64-bit element.
    __epi_1xi64 hipart = __builtin_epi_vsra_1xi64(lz, __builtin_epi_vbroadcast_1xi64(32, vl), vl);

    // Extract the lowest 32 bits of each 64-bit element.
    __epi_1xi64 lowpart = __builtin_epi_vand_1xi64(lz, __builtin_epi_vbroadcast_1xi64(((uint64_t)1 << 32) - 1, vl), vl);
    // If we take the low part, there are 32 or more leading zeros, 
    // then we need to add 32 to the low part.
    lowpart = __builtin_epi_vadd_1xi64(lowpart, __builtin_epi_vbroadcast_1xi64(32, vl), vl);

    //  dest = atleast32lz ? lowpart + 32 : hipart
    lz = __builtin_epi_vmerge_1xi64(hipart, lowpart, atleast32lz, vl);

    return lz;
}

#ifdef DEBUG
char test_8b() {
    unsigned int n = 255;

    uint8_t *src = malloc(n * sizeof(src[0]));
    uint8_t *lz = malloc(n * sizeof(lz[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e8, __epi_m1);

        __epi_8xi8 srci = __builtin_epi_vload_8xi8(src + i, vl);

        __epi_8xi8 lzi = __builtin_epi_softw_vclz_8xi8(srci, vl);

        __builtin_epi_vstore_8xi8(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_clz(src[i]) - 24;
        if (lz[i] != expected) {
            printf("(%u) lz[%u] = %u, expected %u\n", src[i], i, lz[i], expected);
            error = 1;
        }
    }

    free(src);
    free(lz);

    return error;
}

char test_16b() {
    unsigned int n1 = 16;
    unsigned int n2 = 4;
    unsigned int n = n1 * n2;

    uint16_t *src = malloc(n * sizeof(src[0]));
    uint16_t *lz = malloc(n * sizeof(lz[0]));

    unsigned int value = 1;
    for (unsigned int i = 0; i < n1; ++i) {
        for (unsigned int j = 0; j < n2; ++j) {
            src[i * n2 + j] = value + j;
        }
        value <<= 1;
    }
    src[0] = 0;

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e16, __epi_m1);

        __epi_4xi16 srci = __builtin_epi_vload_4xi16(src + i, vl);

        __epi_4xi16 lzi = __builtin_epi_softw_vclz_4xi16(srci, vl);

        __builtin_epi_vstore_4xi16(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_clz(src[i]) - 16;
        if (lz[i] != expected) {
            printf("(%u) lz[%u] = %u, expected %u\n", src[i], i, lz[i], expected);
            error = 1;
        }
    }

    free(src);
    free(lz);

    return error;
}

char test_32b() {
    unsigned int n1 = 32;
    unsigned int n2 = 4;
    unsigned int n = n1 * n2;

    uint32_t *src = malloc(n * sizeof(src[0]));
    uint32_t *lz = malloc(n * sizeof(lz[0]));

    unsigned int value = 1;
    for (unsigned int i = 0; i < n1; ++i) {
        for (unsigned int j = 0; j < n2; ++j) {
            src[i * n2 + j] = value + j;
        }
        value <<= 1;
    }
    src[0] = 0;

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e32, __epi_m1);

        __epi_2xi32 srci = __builtin_epi_vload_2xi32(src + i, vl);

        __epi_2xi32 lzi = __builtin_epi_softw_vclz_2xi32(srci, vl);

        __builtin_epi_vstore_2xi32(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_clz(src[i]);
        if (lz[i] != expected) {
            printf("(%u) lz[%u] = %u, expected %u\n", src[i], i, lz[i], expected);
            error = 1;
        }
    }

    free(src);
    free(lz);

    return error;
}

char test_64b() {
    unsigned int n1 = 64;
    unsigned int n2 = 4;
    unsigned int n = n1 * n2;

    uint64_t *src = malloc(n * sizeof(src[0]));
    uint64_t *lz = malloc(n * sizeof(lz[0]));

    unsigned int value = 1;
    for (unsigned int i = 0; i < n1; ++i) {
        for (unsigned int j = 0; j < n2; ++j) {
            src[i * n2 + j] = value + j;
        }
        value <<= 1;
    }
    src[0] = 0;

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e64, __epi_m1);

        __epi_1xi64 srci = __builtin_epi_vload_1xi64(src + i, vl);

        __epi_1xi64 lzi = __builtin_epi_softw_vclz_1xi64(srci, vl);

        __builtin_epi_vstore_1xi64(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_clzl(src[i]);
        if (lz[i] != expected) {
            printf("(%lu) lz[%u] = %lu, expected %u\n", src[i], i, lz[i], expected);
            error = 1;
        }
    }

    free(src);
    free(lz);

    return error;
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
