#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include "sm4.h"

int main()
{	
	unsigned char  key[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char input[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char pin[128], out[128], output[16];
	unsigned char i, j, length, len, cnt1, cnt2; 
	clock_t start_t, end_t ,total_t;
	sm4_context ctx;
	
 	length = sizeof(input) / sizeof(unsigned char);	
	start_t = clock();	
	
	//明文输出 
	printf("明文：");
	for(i=0; i<length; i++)
		printf("%02x ", input[i]);
	printf("\n");
	
 	while(len < length)
 	{	 	 
		//加密测试
		sm4_setkey_enc(&ctx,key);
		sm4_crypt_ecb(&ctx,1,16,input,output);
		for(i=len,j=0; i<len+16; i++,j++)
		{		
			pin[i] = output[j];
			cnt1++;
		}
			 
		//解密测试
		sm4_setkey_dec(&ctx,key);
		sm4_crypt_ecb(&ctx,0,16,output,output);
		for(i=len,j=0; i<len+16; i++,j++)
		{		
			out[i] = output[j];
			cnt2++;
		}
		
		len = len+16;
		//后推128bit 
		for(i=0; i<16; i++)
		{
			input[i] = input[i+len];
		}	
	}
		
	printf("密文：");
	for(i=0; i<cnt1; i++)
		printf("%02x ", pin[i]);
	printf("\n");
	
	printf("解密：");
	for(i=0; i<cnt2; i++)
		printf("%02x ", out[i]);
	printf("\n");
	
	//计算时间 
	end_t = clock();
	total_t = end_t - start_t;
	printf("time = %d ms", total_t);
}