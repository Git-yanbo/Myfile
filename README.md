# 国密SM4的Hash算法实现

设计要求：  
代码在stm32上实现spi口输入一串数据后经过SM4运算后得到原始数据和最后的hash值打印在屏幕上。尝试统计SM4的运行效率，加速算法运算。

1.SM4算法结构  

* SM4分组算法是一种对称密钥算法，分组长度与密钥长度均为128bit。
* 加密算法与密钥扩展算法均采用非线性迭代结构，运算轮数均为32轮。
* 数据解密和数据加密的算法结构相同，只是轮密钥的使用顺序相反，解密轮密钥是加密轮密钥的逆序。

2.密钥及密钥参量  
  
  ```C
/*********************************
 *函数名：sm4_setkey
 *作用：密钥设置，对当前传入的主密钥进行32轮迭代，每次迭代的轮密钥都存放在ctx结构的sk数组中  
 *参数：sk[32]-存放轮密钥, key[16]-存放主密钥 
 *返回值：无
 *********************************/
void sm4_setkey( unsigned long SK[32], unsigned char key[16] )
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
