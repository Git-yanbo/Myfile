# 国密SM4的Hash算法实现

## 概述

* 代码在stm32上实现spi口输入一串数据后经过SM4运算后得到原始数据和最后的hash值打印在屏幕上。尝试统计SM4的运行效率，加速算法运算。

## 1.SM4算法结构  

* SM4分组算法是一种对称密钥算法，分组长度与密钥长度均为128bit。
* 加密算法与密钥扩展算法均采用非线性迭代结构，运算轮数均为32轮。
* 数据解密和数据加密的算法结构相同，只是轮密钥的使用顺序相反，解密轮密钥是加密轮密钥的逆序。

## 2.密钥及密钥参量

* 密钥长度为128bit，表示为MK = (MK<sub>0</sub>，MK<sub>1</sub>，MK<sub>2</sub>，MK<sub>3</sub>)，其中MK<sub>i</sub> (i=0，1，2，3)为字。
* 轮密钥表示为 (rk<sub>0</sub>，rk<sub>1</sub>，...，rk<sub>32</sub>) 其中rk<sub>i</sub> (i=0，1，2，3) 为bit字,轮密钥由密钥生成。
  
## 3.轮函数

### &emsp;3.1 轮函数结构

* 设输入为 (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>) ∈
Z<sub>2</sub><sup>32</sup>，则轮函数F如下：  
* F (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>，rk) = X<sub>0</sub> ⊕ T (X<sub>1</sub> ⊕ X<sub>2</sub> ⊕ X<sub>3</sub> ⊕ rk)

### &emsp;3.2 合成置换

* T：Z<sub>2</sub><sup>32</sup> → Z<sub>2</sub><sup>32</sup> 是一个可逆变换，由非线性变换τ和线性变换L复合而成。τ由4个并行的S盒构成。设输入为A = (a<sub>0</sub>，a<sub>1</sub>，a<sub>2</sub>，a<sub>3</sub>) ∈ Z<sub>2</sub><sup>32</sup>，输出为B = (b<sub>0</sub>，b<sub>1</sub>，b<sub>2</sub>，b<sub>3</sub>) ∈ Z<sub>2</sub><sup>32</sup>。τ(A)如下:  
* (b<sub>0</sub>，b<sub>1</sub>，b<sub>2</sub>，b<sub>3</sub>) = $\tau$(A) = (Sbox(a<sub>0</sub>)，Sbox(a<sub>1</sub>)，Sbox(a<sub>2</sub>)，Sbox(a<sub>3</sub>))

## 4.加密算法

* 本加密算法由32次迭代运算和1次反序变换R组成。  
* 设明文输入为 (X<sub>0</sub>，X<sub>1</sub>，X<sub>2</sub>，X<sub>3</sub>) ∈ ( Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，密文输出为 (Y<sub>0</sub>，Y<sub>1</sub>，Y<sub>2</sub>，Y<sub>3</sub>) ∈ (Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，轮密钥为 rk<sub>i</sub> ∈ Z<sub>2</sub><sup>32</sup>，i=0.1.2，...，31。加密算法的运算过程如下：  
&emsp;&emsp;1). 32次迭代运算如下：  
&emsp;&emsp;&emsp;&emsp;X<sub>i+4</sub> = F (X<sub>i</sub>，X<sub>i+1</sub>，X<sub>i+2</sub>，X<sub>i+3</sub>，rk<sub>i</sub>)，i=0，1，2，...，31  
&emsp;&emsp;2). 反序变换如下：  
&emsp;&emsp;&emsp;&emsp;(Y<sub>0</sub>，Y<sub>1</sub>，Y<sub>2</sub>，Y<sub>3</sub>) = R (X<sub>32</sub>，X<sub>33</sub>，X<sub>34</sub>，X<sub>35</sub>) = (X<sub>35</sub>，X<sub>34</sub>，X<sub>33</sub>，X<sub>32</sub>)

## 5.解密算法  

* 本算法的解密变换与加密变换结构不同，不同的仅是轮密钥的使用顺序，解密时，使用轮密钥序 (rk<sub>31</sub>，rk<sub>30</sub>，...，rk<sub>0</sub>)。  

## 6.密钥扩展算法  

* 加密过程使用的轮密钥由加密密钥生成，其中加密密钥MK = (MK<sub>0</sub>，MK<sub>1</sub>，MK<sub>2</sub>，MK<sub>3</sub>) ∈ (Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，加密过程中的轮密钥生成方法如下所示：  
&emsp;&emsp;(K<sub>0</sub>，K<sub>1</sub>，K<sub>2</sub>，K<sub>3</sub>) = (MK<sub>0</sub>⊕FK<sub>0</sub>，MK<sub>1</sub>⊕FK<sub>1</sub>，MK<sub>2</sub>⊕FK<sub>2</sub>，MK<sub>3</sub>⊕FK<sub>3</sub>)  
&emsp;&emsp;rk<sub>i</sub> = K<sub>i+4</sub> = K<sub>i</sub>⊕T<sup>'</sup> (K<sub>i+1</sub>⊕K<sub>i+2</sub>⊕K<sub>i+3</sub>⊕ck<sub>i</sub>)，i=0，1，...，31  
&emsp;&emsp;式中：  
&emsp;&emsp;1). T<sup>'</sup>是将合成置换T的线性变换L替换为L<sup>'</sup>，如下：  
&emsp;&emsp;&emsp;&emsp;L<sup>'</sup>(B) = B⊕(B<<<3)⊕(B<<<23)  
&emsp;&emsp;2). 系统参数FK的取值为:  
&emsp;&emsp;&emsp;&emsp;FK<sub>0</sub> = (A3B1BAC6)，FK<sub>1</sub>=(56AA3350)，FK<sub>2</sub>=(677D9197)，FK<sub>3</sub>=(B27022DC)；  
&emsp;&emsp;3). 固定参数CK取值方法为：  
&emsp;&emsp;&emsp;&emsp;设 cK<sub>i+j</sub> 为 
cK<sub>i</sub> 的第j个字节 (i=0,1，...，31；j=0,1,2,3)，即 cK<sub>i</sub> = (cK<sub>i+0</sub>，cK<sub>i+1</sub>，cK<sub>i+2</sub>，cK<sub>i+3</sub>) ∈ (Z<sub>2</sub><sup>8</sup>)<sup>4</sup>，则cK<sub>i+j</sub> = (4i+j)*7(mod25)。  
&emsp;&emsp;&emsp;&emsp;固定参数cK<sub>i</sub>(i=0,1，...，31)具体值为：  
&emsp;&emsp;&emsp;&emsp;00070E15,&emsp; 1C232A31,&emsp;383F464D,&emsp; 545B6269,  
&emsp;&emsp;&emsp;&emsp;70777E85,&emsp; 8C939AA1,&emsp;A8AFB6BD,&emsp;C4CBD2D9,  
&emsp;&emsp;&emsp;&emsp;E0E7EEF5,&emsp; FC030A11,&emsp;181F262D,&emsp; 343B4249,  
&emsp;&emsp;&emsp;&emsp;50575E65,&emsp; 6C737A81,&emsp;888F969D,&emsp; A4ABB2B9,  
&emsp;&emsp;&emsp;&emsp;C0C7CED5,&emsp;DCE3EAF1,&emsp;F8FF060D,&emsp;141B2229,  
&emsp;&emsp;&emsp;&emsp;30373E45,&emsp; 4C535A61,&emsp;686F767D,&emsp;848B9299,  
&emsp;&emsp;&emsp;&emsp;A0A7AEB5,&emsp;BCC3CAD1,&emsp;D8DFE6ED,&emsp;F4FB0209,  
&emsp;&emsp;&emsp;&emsp;10171E25,&emsp; 2C333A41,&emsp;484F565D,&emsp;646B7279。  
* 解密密钥同加密密钥，解密使用的轮密钥由解密密钥生成，其轮密钥生成方法同加密过程的轮密钥生成方法。
