/*
 * SM4 Encryption alogrithm (SMS4 algorithm)
 * GM/T 0002-2012 Chinese National Standard ref:http://www.oscca.gov.cn/ 
 * thanks to Xyssl
 * thnaks and refers to http://hi.baidu.com/numax/blog/item/80addfefddfb93e4cf1b3e61.html
 * author:goldboar
 * email:goldboar@163.com
 * 2012-4-20
 */
 
// Test vector 1
// plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// 	   round key and temp computing result:
// 	   rk[ 0] = f12186f9 X[ 0] = 27fad345
// 		   rk[ 1] = 41662b61 X[ 1] = a18b4cb2
// 		   rk[ 2] = 5a6ab19a X[ 2] = 11c1e22a
// 		   rk[ 3] = 7ba92077 X[ 3] = cc13e2ee
// 		   rk[ 4] = 367360f4 X[ 4] = f87c5bd5
// 		   rk[ 5] = 776a0c61 X[ 5] = 33220757
// 		   rk[ 6] = b6bb89b3 X[ 6] = 77f4c297
// 		   rk[ 7] = 24763151 X[ 7] = 7a96f2eb
// 		   rk[ 8] = a520307c X[ 8] = 27dac07f
// 		   rk[ 9] = b7584dbd X[ 9] = 42dd0f19
// 		   rk[10] = c30753ed X[10] = b8a5da02
// 		   rk[11] = 7ee55b57 X[11] = 907127fa
// 		   rk[12] = 6988608c X[12] = 8b952b83
// 		   rk[13] = 30d895b7 X[13] = d42b7c59
// 		   rk[14] = 44ba14af X[14] = 2ffc5831
// 		   rk[15] = 104495a1 X[15] = f69e6888
// 		   rk[16] = d120b428 X[16] = af2432c4
// 		   rk[17] = 73b55fa3 X[17] = ed1ec85e
// 		   rk[18] = cc874966 X[18] = 55a3ba22
// 		   rk[19] = 92244439 X[19] = 124b18aa
// 		   rk[20] = e89e641f X[20] = 6ae7725f
// 		   rk[21] = 98ca015a X[21] = f4cba1f9
// 		   rk[22] = c7159060 X[22] = 1dcdfa10
// 		   rk[23] = 99e1fd2e X[23] = 2ff60603
// 		   rk[24] = b79bd80c X[24] = eff24fdc
// 		   rk[25] = 1d2115b0 X[25] = 6fe46b75
// 		   rk[26] = 0e228aeb X[26] = 893450ad
// 		   rk[27] = f1780c81 X[27] = 7b938f4c
// 		   rk[28] = 428d3654 X[28] = 536e4246
// 		   rk[29] = 62293496 X[29] = 86b3e94f
// 		   rk[30] = 01cf72e5 X[30] = d206965e
// 		   rk[31] = 9124a012 X[31] = 681edf34
// cypher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46
// 		
// test vector 2
// the same key and plain 1000000 times coumpting 
// plain:  01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// key:    01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
// cypher: 59 52 98 c7 c6 fd 27 1f 04 02 f8 04 c3 3d 3f 66
 
#include "sm4.h"
#include <string.h>
#include <stdio.h>
 
// 将字符型数组b的第i到第i+3位的二进制拼接成一个4*8=32bit的整数，存入n中  
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

// 将整数n的32位的二进制表示转换为4个char的数组，存入数组b的第i到第i+3位 
#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif
 
// SHL(x,n) 可以得到左移 n 位之后的结果，然后与右移的结果 ((x)>>(32-n))逐位或来把右边空缺的n位补齐
#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))

// 数值交换 
#define SWAP(a,b) { unsigned long t = a; a = b; b = t; t = 0; }
 
/*
 * 扩展SM4 S盒
 * Sbox表：8位输入转换为8位输出
 */ 
 
static const unsigned char SboxTable[16][16] = 
{
{0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05},
{0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99},
{0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62},
{0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6},
{0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8},
{0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35},
{0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87},
{0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e},
{0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1},
{0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3},
{0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f},
{0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51},
{0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8},
{0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0},
{0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84},
{0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48}
};
 
/* 系统参数 */
static const unsigned long FK[4] = {0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc};
 
/* 固定参数 */
static const unsigned long CK[32] =
{
0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};
 
 
/*
 * 在SboxTable中查找并获取相关值
 * args:[in] inch: 0x00~0xFF (8位无符号值)
 */
static unsigned char sm4Sbox(unsigned char inch)
{
    unsigned char *pTable = (unsigned char *)SboxTable;
    unsigned char retVal = (unsigned char)(pTable[inch]);
    return retVal;
}
 
/*
 * "T algorithm" == "L algorithm" + "t algorithm"
 * args:[in] a: a是32位无符号值
 * 返回: c使用直线算法“L”和非线性算法“t”计算
 */
static unsigned long sm4Lt(unsigned long ka)
{
    unsigned long bb = 0;
    unsigned long c = 0;
    unsigned char a[4];
	unsigned char b[4];
    PUT_ULONG_BE(ka,a,0)
    b[0] = sm4Sbox(a[0]);
    b[1] = sm4Sbox(a[1]);
    b[2] = sm4Sbox(a[2]);
    b[3] = sm4Sbox(a[3]);
	GET_ULONG_BE(bb,b,0)
    c =bb^(ROTL(bb, 2))^(ROTL(bb, 10))^(ROTL(bb, 18))^(ROTL(bb, 24));
    return c;
}
 
/*
 * 计算并获取加密/解密内容
 * args:[in] x0: 原始内容
 * args:[in] x1: 原始内容
 * args:[in] x2: 原始内容
 * args:[in] x3: 原始内容
 * args:[in] rk: 加密/解密密钥
 * 返回加密/解密内容的内容
 */
static unsigned long sm4F(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long rk)
{
    return (x0^sm4Lt(x1^x2^x3^rk));
}
 
 
/* private function:
 * 计算圆形加密密钥
 * args: [in] a: a是32位无符号值; 
 * return: sk[i]: i{0,1,2,3,...31}.
 * 作用：变换T'，与加密轮函数中的T基本相同，同样是先进行Sbox的非线性替换，
 *       然后进行线性变换，只是线性变换L改为了：rk = bb^(ROTL(bb, 13))^(ROTL(bb, 23));
 */
static unsigned long sm4CalciRK(unsigned long ka)
{
    unsigned long bb = 0;
    unsigned long rk = 0;
    unsigned char a[4];
    unsigned char b[4];
    PUT_ULONG_BE(ka,a,0)
    b[0] = sm4Sbox(a[0]);
    b[1] = sm4Sbox(a[1]);
    b[2] = sm4Sbox(a[2]);
    b[3] = sm4Sbox(a[3]);
	GET_ULONG_BE(bb,b,0)
    rk = bb^(ROTL(bb, 13))^(ROTL(bb, 23));
    return rk;
}

// 密钥设置，对当前传入的主密钥进行32轮迭代，每次迭代的轮密钥都存放在ctx结构的sk数组中 
static void sm4_setkey( unsigned long SK[32], unsigned char key[16] )
{
    unsigned long MK[4];
    unsigned long k[36];
    unsigned long i = 0;
 
 	//通过宏将初始的key密钥转换为4个32位bit整数
    GET_ULONG_BE( MK[0], key, 0 );
    GET_ULONG_BE( MK[1], key, 4 );
    GET_ULONG_BE( MK[2], key, 8 );
    GET_ULONG_BE( MK[3], key, 12 );
    k[0] = MK[0]^FK[0];
    k[1] = MK[1]^FK[1];
    k[2] = MK[2]^FK[2];
    k[3] = MK[3]^FK[3];
    for(; i<32; i++)
    {
    	//对于第i轮的密钥SK[i] ，其是由k[i]和对k[i+1]^k[i+2]^k[i+3]^CK[i]的复合变换T’异或得到的
		//其中CK是固定参数，虽然代码中直接给出了CK，实际上，其是有一定的计算方法的：
		//设CKij为CKi的第j字节，即CKi=（cki0, cki1, cki2, cki3），则ckij=(4i+j)*7(mod 256)
        k[i+4] = k[i] ^ (sm4CalciRK(k[i+1]^k[i+2]^k[i+3]^CK[i]));
        SK[i] = k[i+4];
        
	}
 
}
 
/*
 * 在 sm4_one_round 函数中，先将128位的输入 input[16]数组 转为4个32位的整数，
 * 放入 ulbuf[4] 中，然后迭代调用 static unsigned long sm4F 函数进行32轮加密，
 * 每一轮加密都需要使用之前的128位结果 ulbuf[i]，ulbuf[i+1]，ulbuf[i+2]，ulbuf[i+3]，
 * 和该轮的密钥 sk[i]，产生出该轮的密文 ulbuf[i+4]，最后的密文存储在 ulbuf[35]~ulbuf[32]中，
 * 转换为字符数组的形式存入 output[16]数组中。
 */
static void sm4_one_round( unsigned long sk[32],
                    unsigned char input[16],
                    unsigned char output[16] )
{
    unsigned long i = 0;
    unsigned long ulbuf[36];
 
    memset(ulbuf, 0, sizeof(ulbuf));
    GET_ULONG_BE( ulbuf[0], input, 0 )
    GET_ULONG_BE( ulbuf[1], input, 4 )
    GET_ULONG_BE( ulbuf[2], input, 8 )
    GET_ULONG_BE( ulbuf[3], input, 12 )
    while(i<32)
    {
        ulbuf[i+4] = sm4F(ulbuf[i], ulbuf[i+1], ulbuf[i+2], ulbuf[i+3], sk[i]);
	    i++;
    }
	PUT_ULONG_BE(ulbuf[35],output,0);
	PUT_ULONG_BE(ulbuf[34],output,4);
	PUT_ULONG_BE(ulbuf[33],output,8);
	PUT_ULONG_BE(ulbuf[32],output,12);
}
 
/*
 * SM4 key schedule (128-bit, encryption) SM4关键时刻表
 * 首先调用sm4_setkey_enc(&ctx,key)设置密钥，这个函数会设置mode为加密，
 * 并调用static void sm4_setkey()来完成设置密钥的操作：
 */
void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] )
{
    ctx->mode = SM4_ENCRYPT;
	sm4_setkey( ctx->sk, key );
}
 
// SM4关键时刻表
void sm4_setkey_dec( sm4_context *ctx, unsigned char key[16] )
{
    int i;
	ctx->mode = SM4_ENCRYPT;
    sm4_setkey( ctx->sk, key );
    for( i = 0; i < 16; i ++ )
    {
        SWAP( ctx->sk[ i ], ctx->sk[ 31-i] );
    }
}
 
 
/*
 * SM4-ECB块加密/解密
 * 这个函数的作用是使用ECB模式，是分组密码的一种最基本的工作模式。
 * 根据length的长度来进行循环，每次循环都调用sm4_one_round进行加密或者解密，
 * 到底是加密还是解密，主要是根据第二个参数Mode来进行决定。
 */
 
void sm4_crypt_ecb( sm4_context *ctx,
				   int mode,
				   int length,
				   unsigned char *input,
                   unsigned char *output)
{
    while( length > 0 )
    {
        sm4_one_round( ctx->sk, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }
 
}
 
//SM4-CBC缓冲区加密/解密
void sm4_crypt_cbc( sm4_context *ctx,
                    int mode,
                    int length,
                    unsigned char iv[16],
                    unsigned char *input,
                    unsigned char *output )
{
    int i;
    unsigned char temp[16];
 
    if( mode == SM4_ENCRYPT )
    {
        while( length > 0 )
        {
            for( i = 0; i < 16; i++ )
                output[i] = (unsigned char)( input[i] ^ iv[i] );
 
            sm4_one_round( ctx->sk, output, output );
            memcpy( iv, output, 16 );
 
            input  += 16;
            output += 16;
            length -= 16;
        }
    }
    else /* SM4_DECRYPT */
    {
        while( length > 0 )
        {
            memcpy( temp, input, 16 );
            sm4_one_round( ctx->sk, input, output );
 
            for( i = 0; i < 16; i++ )
                output[i] = (unsigned char)( output[i] ^ iv[i] );
 
            memcpy( iv, temp, 16 );
 
            input  += 16;
            output += 16;
            length -= 16;
        }
    }
}