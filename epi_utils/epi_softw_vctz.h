/**
 * @author Lorién López Villellas (lorien.lopez@bsc.es)
 *
 * @brief 8, 16, 32 and 64 bits vector count trailing zeros functions.
 *
 */

#ifndef EPI_SOFTW_VCTZ_H
#define EPI_SOFTW_VCTZ_H

#include <stdint.h>

// #define DEBUG 1 // For debugging, uncomment DEBUG and change the extension of the file to .c
#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

/**
 * EPI 8-bits vector count trailing zeros.
 *
 * @param src Source 8 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of trailing zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_8xi8 __builtin_epi_softw_vctz_8xi8(__epi_8xi8 src, unsigned long int vl) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup
    // unsigned int v;      // 8-bit word input to count zero bits on right
    // unsigned int c = 8; // c will be the number of zero bits on the right
    // v &= -signed(v);
    // if (v) c--;
    // if (v & 0x0F) c -= 4;
    // if (v & 0x33) c -= 2;
    // if (v & 0x55) c -= 1;

    src = __builtin_epi_vand_8xi8(src, -src, vl);

    __epi_8xi8 vzero = __builtin_epi_vbroadcast_8xi8(0, vl);
    __epi_8xi8 v0x0F = __builtin_epi_vbroadcast_8xi8(0x0F, vl);
    __epi_8xi8 v0x33 = __builtin_epi_vbroadcast_8xi8(0x33, vl);
    __epi_8xi8 v0x55 = __builtin_epi_vbroadcast_8xi8(0x55, vl);

    __epi_8xi1 lessthan4z = __builtin_epi_vmsgtu_8xi8(__builtin_epi_vand_8xi8(src, v0x0F, vl), vzero, vl);
    __epi_8xi1 lessthan2z = __builtin_epi_vmsgtu_8xi8(__builtin_epi_vand_8xi8(src, v0x33, vl), vzero, vl);
    __epi_8xi1 lessthan1z = __builtin_epi_vmsgtu_8xi8(__builtin_epi_vand_8xi8(src, v0x55, vl), vzero, vl);
    __epi_8xi1 zeroz = __builtin_epi_vmsgtu_8xi8(src, vzero, vl);

    __epi_8xi8 tz = __builtin_epi_vbroadcast_8xi8(8, vl); // All elements in the vector are 8
    tz = __builtin_epi_vsub_8xi8_mask(tz, tz, __builtin_epi_vbroadcast_8xi8(1, vl), zeroz, vl);
    tz = __builtin_epi_vsub_8xi8_mask(tz, tz, __builtin_epi_vbroadcast_8xi8(4, vl), lessthan4z, vl);
    tz = __builtin_epi_vsub_8xi8_mask(tz, tz, __builtin_epi_vbroadcast_8xi8(2, vl), lessthan2z, vl);
    tz = __builtin_epi_vsub_8xi8_mask(tz, tz, __builtin_epi_vbroadcast_8xi8(1, vl), lessthan1z, vl);

    return tz;
}

/**
 * EPI 16-bits vector count trailing zeros.
 *
 * @param src Source 16 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of trailing zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_4xi16 __builtin_epi_softw_vctz_4xi16(__epi_4xi16 src, unsigned long int vl) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup
    // unsigned int v;      // 16-bit word input to count zero bits on right
    // unsigned int c = 16; // c will be the number of zero bits on the right
    // v &= -signed(v);
    // if (v) c--;
    // if (v & 0x00FF) c -= 8;
    // if (v & 0x0F0F) c -= 4;
    // if (v & 0x3333) c -= 2;
    // if (v & 0x5555) c -= 1;

    src = __builtin_epi_vand_4xi16(src, -src, vl);

    __epi_4xi16 vzero = __builtin_epi_vbroadcast_4xi16(0, vl);
    __epi_4xi16 v0x00FF = __builtin_epi_vbroadcast_4xi16(0x00FF, vl);
    __epi_4xi16 v0x0F0F = __builtin_epi_vbroadcast_4xi16(0x0F0F, vl);
    __epi_4xi16 v0x3333 = __builtin_epi_vbroadcast_4xi16(0x3333, vl);
    __epi_4xi16 v0x5555 = __builtin_epi_vbroadcast_4xi16(0x5555, vl);

    __epi_4xi1 lessthan8z = __builtin_epi_vmsgtu_4xi16(__builtin_epi_vand_4xi16(src, v0x00FF, vl), vzero, vl);
    __epi_4xi1 lessthan4z = __builtin_epi_vmsgtu_4xi16(__builtin_epi_vand_4xi16(src, v0x0F0F, vl), vzero, vl);
    __epi_4xi1 lessthan2z = __builtin_epi_vmsgtu_4xi16(__builtin_epi_vand_4xi16(src, v0x3333, vl), vzero, vl);
    __epi_4xi1 lessthan1z = __builtin_epi_vmsgtu_4xi16(__builtin_epi_vand_4xi16(src, v0x5555, vl), vzero, vl);
    __epi_4xi1 zeroz = __builtin_epi_vmsgtu_4xi16(src, vzero, vl);

    __epi_4xi16 tz = __builtin_epi_vbroadcast_4xi16(16, vl); // All elements in the vector are 8
    tz = __builtin_epi_vsub_4xi16_mask(tz, tz, __builtin_epi_vbroadcast_4xi16(1, vl), zeroz, vl);
    tz = __builtin_epi_vsub_4xi16_mask(tz, tz, __builtin_epi_vbroadcast_4xi16(8, vl), lessthan8z, vl);
    tz = __builtin_epi_vsub_4xi16_mask(tz, tz, __builtin_epi_vbroadcast_4xi16(4, vl), lessthan4z, vl);
    tz = __builtin_epi_vsub_4xi16_mask(tz, tz, __builtin_epi_vbroadcast_4xi16(2, vl), lessthan2z, vl);
    tz = __builtin_epi_vsub_4xi16_mask(tz, tz, __builtin_epi_vbroadcast_4xi16(1, vl), lessthan1z, vl);

    return tz;
}

/**
 * EPI 32-bits vector count trailing zeros.
 *
 * @param src Source 32 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of trailing zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_2xi32 __builtin_epi_softw_vctz_2xi32(__epi_2xi32 src, unsigned long int vl) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup
    // unsigned int v;      // 32-bit word input to count zero bits on right
    // unsigned int c = 32; // c will be the number of zero bits on the right
    // v &= -signed(v);
    // if (v) c--;
    // if (v & 0x0000FFFF) c -= 16;
    // if (v & 0x00FF00FF) c -= 8;
    // if (v & 0x0F0F0F0F) c -= 4;
    // if (v & 0x33333333) c -= 2;
    // if (v & 0x55555555) c -= 1;

    src = __builtin_epi_vand_2xi32(src, -src, vl);

    __epi_2xi32 vzero = __builtin_epi_vbroadcast_2xi32(0, vl);
    __epi_2xi32 v0x0000FFFF = __builtin_epi_vbroadcast_2xi32(0x0000FFFF, vl);
    __epi_2xi32 v0x00FF00FF = __builtin_epi_vbroadcast_2xi32(0x00FF00FF, vl);
    __epi_2xi32 v0x0F0F0F0F = __builtin_epi_vbroadcast_2xi32(0x0F0F0F0F, vl);
    __epi_2xi32 v0x33333333 = __builtin_epi_vbroadcast_2xi32(0x33333333, vl);
    __epi_2xi32 v0x55555555 = __builtin_epi_vbroadcast_2xi32(0x55555555, vl);

    __epi_2xi1 lessthan16z = __builtin_epi_vmsgtu_2xi32(__builtin_epi_vand_2xi32(src, v0x0000FFFF, vl), vzero, vl);
    __epi_2xi1 lessthan8z  = __builtin_epi_vmsgtu_2xi32(__builtin_epi_vand_2xi32(src, v0x00FF00FF, vl), vzero, vl);
    __epi_2xi1 lessthan4z  = __builtin_epi_vmsgtu_2xi32(__builtin_epi_vand_2xi32(src, v0x0F0F0F0F, vl), vzero, vl);
    __epi_2xi1 lessthan2z  = __builtin_epi_vmsgtu_2xi32(__builtin_epi_vand_2xi32(src, v0x33333333, vl), vzero, vl);
    __epi_2xi1 lessthan1z  = __builtin_epi_vmsgtu_2xi32(__builtin_epi_vand_2xi32(src, v0x55555555, vl), vzero, vl);
    __epi_2xi1 zeroz       = __builtin_epi_vmsgtu_2xi32(src, vzero, vl);

    __epi_2xi32 tz = __builtin_epi_vbroadcast_2xi32(32, vl); // All elements in the vector are 32
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(1, vl), zeroz, vl);
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(16, vl), lessthan16z, vl);
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(8, vl), lessthan8z, vl);
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(4, vl), lessthan4z, vl);
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(2, vl), lessthan2z, vl);
    tz = __builtin_epi_vsub_2xi32_mask(tz, tz, __builtin_epi_vbroadcast_2xi32(1, vl), lessthan1z, vl);

    return tz;
}

/**
 * EPI 64-bits vector count trailing zeros.
 *
 * @param src Source 64 bits vector register.
 * @param vl Vector length (number of elements).
 * @return The number of trailing zeros in each element of the vector register.
 */
#ifndef DEBUG
inline
#endif
__epi_1xi64 __builtin_epi_softw_vctz_1xi64(__epi_1xi64 src, unsigned long int vl) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightModLookup
    // unsigned int v;      // 32-bit word input to count zero bits on right
    // unsigned int c = 32; // c will be the number of zero bits on the right
    // v &= -signed(v);
    // if (v) c--;
    // if (v & 0x0000FFFF) c -= 16;
    // if (v & 0x00FF00FF) c -= 8;
    // if (v & 0x0F0F0F0F) c -= 4;
    // if (v & 0x33333333) c -= 2;
    // if (v & 0x55555555) c -= 1;

    src = __builtin_epi_vand_1xi64(src, -src, vl);

    __epi_1xi64 vzero = __builtin_epi_vbroadcast_1xi64(0, vl);
    __epi_1xi64 v0x00000000FFFFFFFF = __builtin_epi_vbroadcast_1xi64(0x00000000FFFFFFFF, vl);
    __epi_1xi64 v0x0000FFFF0000FFFF = __builtin_epi_vbroadcast_1xi64(0x0000FFFF0000FFFF, vl);
    __epi_1xi64 v0x00FF00FF00FF00FF = __builtin_epi_vbroadcast_1xi64(0x00FF00FF00FF00FF, vl);
    __epi_1xi64 v0x0F0F0F0F0F0F0F0F = __builtin_epi_vbroadcast_1xi64(0x0F0F0F0F0F0F0F0F, vl);
    __epi_1xi64 v0x3333333333333333 = __builtin_epi_vbroadcast_1xi64(0x3333333333333333, vl);
    __epi_1xi64 v0x5555555555555555 = __builtin_epi_vbroadcast_1xi64(0x5555555555555555, vl);

    __epi_1xi1 lessthan32z = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x00000000FFFFFFFF, vl), vzero, vl);
    __epi_1xi1 lessthan16z = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x0000FFFF0000FFFF, vl), vzero, vl);
    __epi_1xi1 lessthan8z  = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x00FF00FF00FF00FF, vl), vzero, vl);
    __epi_1xi1 lessthan4z  = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x0F0F0F0F0F0F0F0F, vl), vzero, vl);
    __epi_1xi1 lessthan2z  = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x3333333333333333, vl), vzero, vl);
    __epi_1xi1 lessthan1z  = __builtin_epi_vmsgtu_1xi64(__builtin_epi_vand_1xi64(src, v0x5555555555555555, vl), vzero, vl);
    __epi_1xi1 zeroz       = __builtin_epi_vmsgtu_1xi64(src, vzero, vl);

    __epi_1xi64 tz = __builtin_epi_vbroadcast_1xi64(64, vl); // All elements in the vector are 32
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(1, vl), zeroz, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(32, vl), lessthan32z, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(16, vl), lessthan16z, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(8, vl), lessthan8z, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(4, vl), lessthan4z, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(2, vl), lessthan2z, vl);
    tz = __builtin_epi_vsub_1xi64_mask(tz, tz, __builtin_epi_vbroadcast_1xi64(1, vl), lessthan1z, vl);

    return tz;
}


#ifdef DEBUG
char test_8b() {
    unsigned int n = 255;

    uint8_t *src = malloc(n * sizeof(src[0]));
    uint8_t *tz = malloc(n * sizeof(tz[0]));

    for (unsigned int i = 0; i < n; i++) {
        src[i] = i;
    }

    unsigned long int vl = 0;
    for (unsigned int i = 0; i < n; i += vl) {
        vl =  __builtin_epi_vsetvl(n - i, __epi_e8, __epi_m1);

        __epi_8xi8 srci = __builtin_epi_vload_8xi8(src + i, vl);

        __epi_8xi8 tzi = __builtin_epi_softw_vctz_8xi8(srci, vl);

        __builtin_epi_vstore_8xi8(tz + i, tzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_ctz(src[i]);
        expected = expected > 8 ? 8 : expected;
        if (tz[i] != expected) {
            printf("(%u) tz[%u] = %u, expected %u\n", src[i], i, tz[i], expected);
            error = 1;
        }
    }

    free(src);
    free(tz);

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

        __epi_4xi16 lzi = __builtin_epi_softw_vctz_4xi16(srci, vl);

        __builtin_epi_vstore_4xi16(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_ctz(src[i]);
        expected = expected > 16 ? 16 : expected;
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

        __epi_2xi32 lzi = __builtin_epi_softw_vctz_2xi32(srci, vl);

        __builtin_epi_vstore_2xi32(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_ctz(src[i]);
        expected = expected > 32 ? 32 : expected;
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

        __epi_1xi64 lzi = __builtin_epi_softw_vctz_1xi64(srci, vl);

        __builtin_epi_vstore_1xi64(lz + i, lzi, vl);
    }

    char error = 0;
    for (unsigned int i = 0; i < n; ++i) {
        unsigned int expected = __builtin_ctzl(src[i]);
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