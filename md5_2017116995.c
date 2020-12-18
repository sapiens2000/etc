#define _CRT_SECURE_NO_WARNINGS

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | ~z))


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


const uint32_t T[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
};

const uint32_t s[] = {
7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 
};


/* 초기값 저장 */
uint32_t h0, h1, h2, h3;

void md5(uint8_t* msg) {
    uint32_t* w;
    uint32_t a, b, c, d, f, g, temp;
    int i = 0;


    

    /* 초기값 little endian */
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;

    w = (uint32_t*)(msg);

    a = h0;
    b = h1;
    c = h2;
    d = h3;

    /* 64 operations */
    for (i = 0; i < 64; i++) {

        if (i < 16) {
            f = F(b, c, d);
            g = i;
        }
        else if (i < 32) {
            f = G(b, c, d);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48) {
            f = H(b, c, d);
            g = (3 * i + 5) % 16;
        }
        else {
            f = I(b, c, d);
            g = (7 * i) % 16;
        }

        temp = d;
        d = c;
        c = b;
        b = b + LEFTROTATE((a + f + T[i] + w[g]), s[i]);
        a = temp;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;

}

void md5_puthash(uint32_t hash, uint8_t* dst) {
    /* hash 값을 차례대로 배열에 저장 */
    if (!dst) return;

    dst[0] = (uint8_t)(hash);
    dst[1] = (uint8_t)(hash >> 8);
    dst[2] = (uint8_t)(hash >> 16);
    dst[3] = (uint8_t)(hash >> 24);
}

int main() {
    int i;
    int num = 0;
    /* 64 MSG */
    unsigned char msg[64] = { 0, };
    uint8_t digest[16] = { 0, };

    printf("MESSAGE : \n");
    
    /* 입력파일에 있는 문자열 전체를 그대로 입력하는 경우*/
    for (i = 0; i < 64; i++) {
        scanf_s("%x", &num);
        /* 스페이스바 버리기 */
        getchar();
        msg[i] = (unsigned char)num;
    }

    md5((uint8_t*)msg);

    /* 계산된 결과값을 digest 배열에 저장 */
    md5_puthash(h0, digest);
    md5_puthash(h1, digest + 4);
    md5_puthash(h2, digest + 8);
    md5_puthash(h3, digest + 12);

    printf("\nMD5 : ");

    for (int i = 0; i < 16; i++) printf("%02x", digest[i]);

    puts("");
    for (int i = 0; i < 64; i++) printf("%02x ",msg[i]);


    return 0;
}
