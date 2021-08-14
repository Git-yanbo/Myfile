# SM4算法实现分析

## 1.算法调用参数

&emsp;定义一个结构体 sm4_context ctx 用于保存上下文信息。其中 mode 为加密/解密，sk[32] 数组存放轮密钥。

```C
typedef struct
{
    int mode;                   
    unsigned long sk[32];      
}
sm4_context;
```

## 2.密钥调度算法

&emsp;设定 ctx 的模式为加密，调用 sm4_setkey 函数设置密钥。

```C
void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] )
{
    ctx->mode = SM4_ENCRYPT;
    sm4_setkey( ctx->sk, key );
}
```

## 3.密钥设置算法

&emsp;对当前传入的主密钥进行32轮迭代，每次迭代的轮密钥都存放在 ctx 结构的 sk 数组中。

```C
static void sm4_setkey( unsigned long SK[32], unsigned char key[16] )
{
    unsigned long MK[4];
    unsigned long k[36];
    unsigned long i = 0;
 
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
        k[i+4] = k[i] ^ (sm4CalciRK(k[i+1]^k[i+2]^k[i+3]^CK[i]));
        SK[i] = k[i+4];
    }
}
```

&emsp;通过宏 GET_ULONG_BE 将初始的密钥转换为4个32位bit整数。MK0，MK1，MK2，MK3，为计算各轮密钥预先准备好初始值。  
&emsp;FK 数组为系统参数，系统参数 FK 的取值为:

```C
FK0 = (A3B1BAC6)，FK1=(56AA3350)，FK2=(677D9197)，FK3=(B27022DC)；
```

&emsp;此后，对于第i轮的密钥 SK[i]，都是由 k[i] 和对 k[i+1]^k[i+2]^k[i+3]^CK[i] 的复合变换 T' 异或得到的。

```C
 k[i+4] = k[i] ^ (sm4CalciRK(k[i+1]^k[i+2]^k[i+3]^CK[i]));
 ```

&emsp;其中 CK[i] 是固定参数，设 CK<sub>i+j</sub> 为 CK<sub>i</sub> 的第 j 个字节即 CK<sub>i</sub> = (CK<sub>i+0</sub>，CK<sub>i+1</sub>，CK<sub>i+2</sub>，CK<sub>i+3</sub>) ，则CK<sub>i+j</sub> = (4i+j)*7(mod25)。

```C
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
```

&emsp;sm4CalciRK函数即变换T'，与加密轮函数中的T基本相同，同样是先进行Sbox的非线性替换，然后进行线性变换，只是线性变换L改为 rk = bb^(ROTL(bb, 13))^(ROTL(bb, 23));

```C
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
```

## 4.加密过程

&emsp;采用ECB(电码本)模式，将数据按照8个字节一段进行DES加密得到一段段的8字节的密文，最后一段不足8个字节(一般补0或者F)，按照需求补足8个字节进行并行计算，之后按照顺序将计算所得的数据连在一起即可，各段数据之间互不影响。  
&emsp;sm4_crypt_ecb 函数根据 length 的长度来进行循环，每次循环都调用 sm4_one_round 函数对每一块密文进行加密。

```C
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
```

&emsp;GET_ULONG_BE(n,b,i) 的作用是将字符型数组b的第 i 位到第 i+3 位的二进制拼接成一个4*8=32bit的整数，存入n中。  
&emsp;PUT_ULONG_BE(n,b,i) 的作用是将整数n的32位二进制转换为4个 char 类型的数组，存入数组b的第 i 位到第 i+3 位。

```C
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}

#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
```

&emsp;在 sm4_one_round 函数中，先将128位的输入 input[16]数组 转为4个32位的整数，放入 ulbuf[4] 中，然后迭代调用 static unsigned long sm4F 函数进行32轮加密，每一轮加密都需要使用之前的128位结果 ulbuf[i]，ulbuf[i+1]，ulbuf[i+2]，ulbuf[i+3]，和该轮的密钥 sk[i]，产生出该轮的密文 ulbuf[i+4]，最后的密文存储在 ulbuf[35]~ulbuf[32]中，转换为字符数组的形式存入 output[16]数组中。

```C
static unsigned long sm4F(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long rk)
{
    return (x0^sm4Lt(x1^x2^x3^rk));
}
```

&emsp;每一轮加密中，输入为(x0,x1,x2,x3)，xi为32比特，共计128比特。通过 x0^sm4Lt(x1^x2^x3^rk) 得到该轮加密的结果。在此时，SM4就将轮密钥应用在了加密中。  
&emsp;sm4Lt 函数是一个合成变换，由非线性变换 t 和线性变换 L 复合而成。首先将输入的整数ka转换为8比特一个的字符 PUT_ULONG_BE(ka,a,0)，然后使用S盒进行非线性变换，再将变换结果转换为32比特的整数 GET_ULONG_BE(bb,b,0)，最后对得到的32位整数bb进行线性变换 c =bb^(ROTL(bb, 2))^(ROTL(bb, 10))^(ROTL(bb, 18))^(ROTL(bb, 24))，从而得到复合变换的结果c。

```C
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
```

&emsp;算法中对循环位移的实现十分巧妙，SHL(x,n) 可以得到左移 n 位之后的结果，然后与右移的结果 ((x)>>(32-n))逐位或来把右边空缺的n位补齐，效率很高。

```C
#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))
```

&emsp;SM4加密先使用sbox进行非线性变换，然后通过循环移位操作进行线性变换。其每轮加密用到了中之前4轮加密的结果，进一步提高了加密的强度。

## 5.解密过程

&emsp;解密时需调用 sm4_setkey_dec 函数将密钥的顺序进行倒置，然后调用 sm4_crypt_ecb 即可解密。实际上SM4的解密变换与加密变换结构相同，不同的仅仅是轮密钥的使用顺序相反。

```C
#define SWAP(a,b) { unsigned long t = a; a = b; b = t; t = 0; }

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
```
