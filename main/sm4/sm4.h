/**
 * \file sm4.h
 */
#ifndef XYSSL_SM4_H
#define XYSSL_SM4_H
 
#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
 
/**
 * \brief SM4 context structure SM4上下文结构简介
 */
typedef struct
{
    int mode;                   /*!<  加密/解密   */
    unsigned long sk[32];       /*!<  轮密钥      */
}
sm4_context;
 
 
#ifdef __cplusplus
extern "C" {
#endif
 
/**
 * \brief          SM4 key schedule (128-bit, encryption) 	\简要SM4密钥计划（128位，加密）
 *
 * \param ctx      SM4 context to be initialized 			\要初始化的参数ctx SM4上下文
 * \param key      16-byte secret key 						\参数密钥16字节密钥
 */
void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] );
 
/**
 * \brief          SM4 key schedule (128-bit, decryption) 	\简要SM4密钥计划（128位，解密）
 *
 * \param ctx      SM4 context to be initialized 			\要初始化的参数ctx SM4上下文
 * \param key      16-byte secret key 						\参数密钥16字节密钥
 */
void sm4_setkey_dec( sm4_context *ctx, unsigned char key[16] );
 
/**
 * \brief          SM4-ECB block encryption/decryption		\简短///SM4-ECB块加密/解密
 * \param ctx      SM4 context								\参数ctx+SM4上下文
 * \param mode     SM4_ENCRYPT or SM4_DECRYPT				\参数模式SM4_加密或SM4_解密
 * \param length   length of the input data					\参数长度输入数据的长度
 * \param input    input block								\参数输入块
 * \param output   output block								\参数输出块
 */
void sm4_crypt_ecb( sm4_context *ctx,
				     int mode,
					 int length,
                     unsigned char *input,
                     unsigned char *output);
 
/**
 * \brief          SM4-CBC buffer encryption/decryption		\简短///SM4-CBC块加密/解密
 * \param ctx      SM4 context								\参数ctx+SM4上下文
 * \param mode     SM4_ENCRYPT or SM4_DECRYPT				\参数模式SM4_加密或SM4_解密
 * \param length   length of the input data					\参数长度输入数据的长度
 * \param iv       initialization vector (updated after use)\参数iv初始化向量（使用后更新）
 * \param input    buffer holding the input data			\保存输入数据的参数输入缓冲区
 * \param output   buffer holding the output data			\保存输出数据的缓冲区
 */
void sm4_crypt_cbc( sm4_context *ctx,
                     int mode,
                     int length,
                     unsigned char iv[16],
                     unsigned char *input,
                     unsigned char *output );
 
#ifdef __cplusplus
}
#endif
 
#endif