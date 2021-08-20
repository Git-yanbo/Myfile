### SM4
#### 1. 算法概述

##### 1.1 简介

SM4 是中国采用的一种分组密码标准。在密码学中，分组加密是一种对称密钥算法，它将明文分成多个等长的模块，使用确定的算法和对称密钥对每组分别加密解密。其分组长度与密钥长度均为 128bit，加密算法与密钥扩展算法都采用 32 轮非线性迭代结构，S 盒为固定的 8bit 输入 8bit输出。

##### 1.2 优势：

轮变换使用的模块（异或运算、S盒、32bit的线性置换）非常适合32位处理器实现。

加密过程与解密过程相同，只是轮密钥使用顺序正好相反，所以它不仅适用于软件编程实现，更适合硬件芯片实现。

##### 1.3 应用场景：

在商用体系中，SM4 主要用于数据加密，应用领域从电子邮件加密到银行交易转帐，非常广泛。

#### 2. 算法流程

##### 2.1 总体框图：



![SM4](C:\Users\Administrator\Documents\SM4.jpg)



##### 2.2 密钥及密钥参量

<font color=#008000>密钥表示为rk</font> = (MK<sub>0</sub>，MK<sub>1</sub>，MK<sub>2</sub>，MK<sub>3</sub>)，长度为128bit，其中MK<sub>i</sub> (i=0，1，2，3)为32bit字。

<font color=#008000>轮密钥表示为rk</font> =  (rk<sub>0</sub>，rk<sub>1</sub>，...，rk<sub>32</sub>) 其中rk<sub>i</sub> (i=0，1，2，3) 为32bit字，轮密钥由密钥生成。轮密钥的生成公式如(1-1)、(1-2)所示：

(K<sub>0</sub>，K<sub>1</sub>，K<sub>2</sub>，K<sub>3</sub>) = (MK<sub>0 </sub>⊕ FK<sub>0</sub>，MK<sub>1</sub> ⊕ FK<sub>1</sub>，MK<sub>2</sub> ⊕ FK<sub>2</sub>，MK<sub>3 </sub>⊕ FK<sub>3</sub>)   ···········(1-1)

rk<sub>i</sub> = K<sub>i+4</sub> = K<sub>i</sub> ⊕ T<sup>'</sup> (K<sub>i+1 </sub>⊕ K<sub>i+2 </sub>⊕ K<sub>i+3 </sub>⊕ ck<sub>i</sub>)   ····························································(1-2)

**2.2.1 T' 是将合成置换 T 的线性变换 L 替换成 L'** ，公式如(1-3)所示：

​		L'(B) = B ⊕ (B<<<13) ⊕ (B<<<23) ·····································································(1-3)

**2.2.2 系统参数 <font color=#008000> FK </font> 的取值为：**

​		FK<sub>0</sub> = (A3B1BAC6)，FK<sub>1</sub>=(56AA3350)，FK<sub>2</sub>=(677D9197)，FK<sub>3</sub>=(B27022DC)； 

**2.2.3 固定参数cK<sub>i</sub>(i=0,1，...，31)具体值为：**  

​		00070E15，1C232A31，383F464D，545B6269,  

​		70777E85，8C939AA1，A8AFB6BD，C4CBD2D9,  

​		E0E7EEF5，FC030A11，181F262D，343B4249,  

​		50575E65，6C737A81，888F969D，A4ABB2B9,  

​		C0C7CED5，DCE3EAF1，F8FF060D，141B2229,  

​		30373E45，4C535A61，686F767D，848B9299,  

​		A0A7AEB5，BCC3CAD1，D8DFE6ED，F4FB0209,  

​		10171E25，2C333A41，484F565D，646B7279。  

##### 2.3 轮函数结构

设输入为 (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>) ∈Z<sub>2</sub><sup>32</sup>，则<font color=#008000>轮函数 F </font>如式(1-4)所示：  

F (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>，rk) = X<sub>0</sub> ⊕ T (X<sub>1</sub> ⊕ X<sub>2</sub> ⊕ X<sub>3</sub> ⊕ rk)  ············································(1-4)

##### 2.4 合成置换T

T：Z<sub>2</sub><sup>32</sup> → Z<sub>2</sub><sup>32</sup> 是一个可逆变换，由非线性变换τ和线性变换L复合而成。即 T(.)=L (τ(.)) 。

##### 2.5 非线性置换τ

τ  由4个并行的 S 盒构成。

设输入为A = (a<sub>0</sub>，a<sub>1</sub>，a<sub>2</sub>，a<sub>3</sub>) ∈ Z<sub>2</sub><sup>32</sup>，输出为B = (b<sub>0</sub>，b<sub>1</sub>，b<sub>2</sub>，b<sub>3</sub>) ∈ Z<sub>2</sub><sup>32</sup>。τ(A) 式(1-5)所示：

(b<sub>0</sub>，b<sub>1</sub>，b<sub>2</sub>，b<sub>3</sub>) = τ(A) = (Sbox(a<sub>0</sub>)，Sbox(a<sub>1</sub>)，Sbox(a<sub>2</sub>)，Sbox(a<sub>3</sub>)) ·················(1-5)

##### 2.6 线性变换L

非线性变换 τ 的输出是线性变换 L 的输入。设输入为B ∈ Z<sub>2</sub><sup>32</sup>，输出为C ∈ Z<sub>2</sub><sup>32</sup>，则C如下：

C = L(B) = B ⊕ (B<<<2) ⊕(B <<< 10) ⊕ (B<<<18) ⊕(B<<<24)  ································(1-6)

##### 2.7 加密算法

本加密算法由 32 次迭代运算和1次反序变换 R 组成。  

设明文输入为 (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>) ∈ ( Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，密文输出为 (Y<sub>0</sub>，Y<sub>1</sub>，Y<sub>2</sub>，Y<sub>3</sub>) ∈ (Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，轮密钥为 rk<sub>i</sub> ∈ Z<sub>2</sub><sup>32</sup>，i=0.1.2，...，31。加密算法的运算过程如式(1-7)、(1-8)所示： 

1). 32次迭代运算如下：  

X<sub>i+4</sub> = F (X<sub>i</sub>，X<sub>i+1</sub>，X<sub>i+2</sub>，X<sub>i+3</sub>，rk<sub>i</sub>)，i=0，1，2，...，31    ······································(1-7)

2). 反序变换如下：  

(Y<sub>0</sub>，Y<sub>1</sub>，Y<sub>2</sub>，Y<sub>3</sub>) = R (X<sub>32</sub>，X<sub>33</sub>，X<sub>34</sub>，X<sub>35</sub>) = (X<sub>35</sub>，X<sub>34</sub>，X<sub>33</sub>，X<sub>32</sub>)   ····················(1-8)

##### 2.8 解密算法

本算法的解密变换与加密变换结构不同，不同的仅是轮密钥的使用顺序，解密时，使用轮密钥序 (rk<sub>31</sub>，rk<sub>30</sub>，...，rk<sub>0</sub>)。解密密钥同加密密钥，解密使用的轮密钥由解密密钥生成，其轮密钥生成方法同加密过程的轮密钥生成方法。  

#### 3. 算法实现

##### 3.1 算法调用参数

定义一个结构体 sm4_context ctx 用于保存上下文信息。其中 mode 为加密/解密，sk[32] 数组存放轮密钥。

```C
typedef struct
{
    int mode;                   
    unsigned long sk[32];      
}
sm4_context;
```

##### 3.2 密钥调度算法

设定 ctx 的模式为加密，调用 sm4_setkey 函数设置密钥。

```C
void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] )
{
    ctx->mode = SM4_ENCRYPT;
    sm4_setkey( ctx->sk, key );
}
```

##### 3.3 密钥设置算法

对当前传入的主密钥进行32轮迭代，每次迭代的轮密钥都存放在 ctx 结构的 sk 数组中。

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

通过宏 GET_ULONG_BE 将初始的密钥转换为4个32位bit整数。MK0，MK1，MK2，MK3，为计算各轮密钥预先准备好初始值。  
&emsp;FK 数组为系统参数，系统参数 FK 的取值为:

```C
FK0 = (A3B1BAC6)，FK1=(56AA3350)，FK2=(677D9197)，FK3=(B27022DC)；
```

此后，对于第i轮的密钥 SK[i]，都是由 k[i] 和对 k[i+1]^k[i+2]^k[i+3]^CK[i] 的复合变换 T' 异或得到的。

```C
 k[i+4] = k[i] ^ (sm4CalciRK(k[i+1]^k[i+2]^k[i+3]^CK[i]));
```

其中 CK[i] 是固定参数，设 CK<sub>i+j</sub> 为 CK<sub>i</sub> 的第 j 个字节即 CK<sub>i</sub> = (CK<sub>i+0</sub>，CK<sub>i+1</sub>，CK<sub>i+2</sub>，CK<sub>i+3</sub>) ，则CK<sub>i+j</sub> = (4i+j)*7(mod25)。

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

sm4CalciRK函数即变换T'，与加密轮函数中的T基本相同，同样是先进行Sbox的非线性替换，然后进行线性变换，只是线性变换L改为 rk = bb^(ROTL(bb, 13))^(ROTL(bb, 23));

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

##### 3.4 加密过程

采用ECB(电码本)模式，将数据按照8个字节一段进行DES加密得到一段段的8字节的密文，最后一段不足8个字节(一般补0或者F)，按照需求补足8个字节进行并行计算，之后按照顺序将计算所得的数据连在一起即可，各段数据之间互不影响。  

sm4_crypt_ecb 函数根据 length 的长度来进行循环，每次循环都调用 sm4_one_round 函数对每一块密文进行加密。

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

GET_ULONG_BE(n,b,i) 的作用是将字符型数组b的第 i 位到第 i+3 位的二进制拼接成一个4*8=32bit的整数，存入n中。  

PUT_ULONG_BE(n,b,i) 的作用是将整数n的32位二进制转换为4个 char 类型的数组，存入数组b的第 i 位到第 i+3 位。

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

在 sm4_one_round 函数中，先将128位的输入 input[16]数组 转为4个32位的整数，放入 ulbuf[4] 中，然后迭代调用 static unsigned long sm4F 函数进行32轮加密，每一轮加密都需要使用之前的128位结果 ulbuf[i]，ulbuf[i+1]，ulbuf[i+2]，ulbuf[i+3]，和该轮的密钥 sk[i]，产生出该轮的密文 ulbuf[i+4]，最后的密文存储在 ulbuf[35]~ulbuf[32]中，转换为字符数组的形式存入 output[16]数组中。

```C
static unsigned long sm4F(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3, unsigned long rk)
{
    return (x0^sm4Lt(x1^x2^x3^rk));
}
```

每一轮加密中，输入为(x0,x1,x2,x3)，xi为32比特，共计128比特。通过 x0^sm4Lt(x1^x2^x3^rk) 得到该轮加密的结果。在此时，SM4就将轮密钥应用在了加密中。

sm4Lt 函数是一个合成变换，由非线性变换 t 和线性变换 L 复合而成。首先将输入的整数ka转换为8比特一个的字符 PUT_ULONG_BE(ka,a,0)，然后使用S盒进行非线性变换，再将变换结果转换为32比特的整数 GET_ULONG_BE(bb,b,0)，最后对得到的32位整数bb进行线性变换 c =bb^(ROTL(bb, 2))^(ROTL(bb, 10))^(ROTL(bb, 18))^(ROTL(bb, 24))，从而得到复合变换的结果c。

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

算法中对循环位移的实现十分巧妙，SHL(x,n) 可以得到左移 n 位之后的结果，然后与右移的结果 ((x)>>(32-n))逐位或来把右边空缺的n位补齐，效率很高。

```C
#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))
```

SM4加密先使用sbox进行非线性变换，然后通过循环移位操作进行线性变换。其每轮加密用到了中之前4轮加密的结果，进一步提高了加密的强度。

##### 3.5 解密过程

解密时需调用 sm4_setkey_dec 函数将密钥的顺序进行倒置，然后调用 sm4_crypt_ecb 即可解密。实际上SM4的解密变换与加密变换结构相同，不同的仅仅是轮密钥的使用顺序相反。

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

#### 4. 算法测试

##### 4.1 Windows10 测试

![image-20210820123321947](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20210820123321947.png)

```C
//开始计时 
start_t = clock(); 
while (fgets(input, MAX_SIZE, infile) !=NULL)
{
    //加密测试
	sm4_setkey_enc(&ctx,key);
	sm4_crypt_ecb(&ctx,1,16,input,output);	
	
	//解密测试
	sm4_setkey_dec(&ctx,key);
	sm4_crypt_ecb(&ctx,0,16,output,output);
	
	fputs (output, outfile) ; //把字符串输出到文件 
}
//计算时间
end_t = clock();
```

![image-20210820140841114](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20210820140841114.png)

​			大小 = 400k							速度  = 5.33 M/S

##### 4.2 STM32F1 测试

![stm32](C:\Users\Administrator\Documents\Visio\stm32.jpg)

![image-20210820143954608](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20210820143954608.png)

数据大小 = 128bit  		 时间 = 10810us     	速度 = 12.5 k/s

