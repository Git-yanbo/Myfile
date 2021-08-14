# 国密SM4的Hash算法实现

## 设计要求

代码在stm32上实现spi口输入一串数据后经过SM4运算后得到原始数据和最后的hash值打印在屏幕上。尝试统计SM4的运行效率，加速算法运算。

## 1.SM4算法结构  

* SM4分组算法是一种对称密钥算法，分组长度与密钥长度均为128bit。
* 加密算法与密钥扩展算法均采用非线性迭代结构，运算轮数均为32轮。
* 数据解密和数据加密的算法结构相同，只是轮密钥的使用顺序相反，解密轮密钥是加密轮密钥的逆序。

## 2.密钥及密钥参量

* 密钥长度为128bit，表示为MK = ($MK_0$，$MK_1$，$MK_2$，$MK_3$)，其中$MK_i$(i=0，1，2，3)为字。
* 轮密钥表示为($rk_0$，$rk_0$，...，rk<sub>32</sub>其中$rk_i$(i=0，1，2，3)为bit字,轮密钥由密钥生成。
  
## 3.轮函数

### &emsp;&emsp;3.1 轮函数结构

&emsp;&emsp;&emsp;设输入为（$X_0$，$X_1$，$X_2$，$X_3$）∈
Z<sub>2</sub><sup>32</sup>，则轮函数F如下：  
&emsp;&emsp;&emsp;F($X_0$，$X_1$，$X_2$，$X_3$，rk)=$X_0$$\bigoplus$T($X_1$$\bigoplus$$X_2$$\bigoplus$$X_3$$\bigoplus$rk)

### &emsp;&emsp;3.2 合成置换

&emsp;&emsp;&emsp;T:Z<sub>2<sup>32</sub>$\rightarrow$Z<sub>2<sup>32d</sub>是一个可逆变换，由非线性变换$\tau$和线性变换L复合而成。$\tau$由4个并行的S盒构成。设输入为A=($a_0$，$a_1$，$a_2$，$a_3$)∈Z<sub>2</sub><sup>32</sup>，输出为B=($b_0$，$b_1$，$b_2$，$b_3$)∈Z<sub>2</sub><sup>32</sup>。$\tau$(A)如下:  
&emsp;&emsp;&emsp;($b_0$，$b_1$，$b_2$，$b_3$)=$\tau$(A)=(Sbox($a_0$)，Sbox($a_1$)，Sbox($a_2$)，Sbox($a_3$))

## 4.加密算法

&emsp;&emsp;&emsp;本加密算法由32次迭代运算和1次反序变换R组成。  
&emsp;&emsp;&emsp;设明文输入为($X_0$，$X_1$，$X_2$，$X_3$)∈(Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，密文输出为($Y_0$，$Y_1$，$Y_2$，$Y_3$)∈(Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，轮密钥为$rk_i$∈Z<sub>2</sub><sup>32</sup>，i=0.1.2，...，31。加密算法的运算过程如下：  
&emsp;&emsp;&emsp;1)&emsp;32次迭代运算如下：  
&emsp;&emsp;&emsp;&emsp;X<sub>i+4</sub>=F(X<sub>i</sub>，X<sub>i+1</sub>，X<sub>i+2</sub>，X<sub>i+3</sub>，rk<sub>i</sub>)，i=0，1，2，...，31  
&emsp;&emsp;&emsp;2)&emsp;反序变换如下：  
&emsp;&emsp;&emsp;&emsp;($Y_0$，$Y_1$，$Y_2$，$Y_3$)=R(X<sub>32</sub>，X<sub>33</sub>，X<sub>34</sub>，X<sub>35</sub>)=(X<sub>35</sub>，X<sub>34</sub>，X<sub>33</sub>，X<sub>32</sub>)

## 5.解密算法  

&emsp;&emsp;&emsp;本算法的解密变换与加密变换结构不同，不同的仅是轮密钥的使用顺序，解密时，使用轮密钥序(rk<sub>31</sub>，rk<sub>30</sub>，...，rk<sub>0</sub>)。  

## 6.密钥扩展算法  

&emsp;&emsp;&emsp;加密过程使用的轮密钥由加密密钥生成，其中加密密钥MK=($MK_0$，$MK_1$，$MK_、2$，$MK_3$)∈(Z<sub>2</sub><sup>32</sup>)<sup>4</sup>，加密过程中的轮密钥生成方法如下所示：  
&emsp;&emsp;&emsp;($K_0$，$K_1$，$K_2$，$K_3$)=($MK_0$$\bigoplus$$FK_0$，$MK_1$$\bigoplus$$FK_1$，$MK_2$$\bigoplus$$FK_2$，$MK_3$$\bigoplus$$FK_3$)  
&emsp;&emsp;&emsp;$rk_i$=K<sub>i+4</sub>=K<sub>i</sub>$\bigoplus$T<sup>'</sup>(K<sub>i+1</sub>$\bigoplus$K<sub>i+2</sub>$\bigoplus$K<sub>i+3</sub>$\bigoplus$ck<sub>i</sub>)，i=0，1，...，31  
&emsp;&emsp;&emsp;式中：  
&emsp;&emsp;&emsp;1)&emsp;T<sup>'</sup>是将合成置换T的线性变换L替换为L<sup>'</sup>，如下：  
&emsp;&emsp;&emsp;&emsp; L<sup>'</sup>(B)=B$\bigoplus$(B<<<3)$\bigoplus$(B<<<23)  
&emsp;&emsp;&emsp;2)&emsp;系统参数FK的取值为:  
&emsp;&emsp;&emsp;&emsp;$FK_0$=(A3B1BAC6)，$FK_1$=(56AA3350)，$FK_2$=(677D9197)，$FK_3$=(B27022DC)；  
&emsp;&emsp;&emsp;3)&emsp;固定参数CK取值方法为：  
&emsp;&emsp;&emsp;&emsp;设cK<sub>i+j</sub>为
cK<sub>i</sub>的第j个字节(i=0,1，...，31；j=0,1,2,3)，即cK<sub>i</sub>=(cK<sub>i+0</sub>，cK<sub>i+1</sub>，cK<sub>i+2</sub>，cK<sub>i+3</sub>)∈(Z<sub>2</sub><sup>8</sup>)<sup>4</sup>，则cK<sub>i+j</sub>=(4i+j)*7(mod25)。  
&emsp;&emsp;&emsp;&emsp;固定参数cK<sub>i</sub>(i=0,1，...，31)具体值为：  
&emsp;&emsp;&emsp;&emsp;00070E15,1C232A31,383F464D,545B6269,  
&emsp;&emsp;&emsp;&emsp;70777E85,8C939AA1,A8AFB6BD,C4CBD2D9,  
&emsp;&emsp;&emsp;&emsp;E0E7EEF5,FC030A11,181F262D,343B4249,  
&emsp;&emsp;&emsp;&emsp;50575E65,6C737A81,888F969D,A4ABB2B9,  
&emsp;&emsp;&emsp;&emsp;C0C7CED5,DCE3EAF1,F8FF060D,141B2229,  
&emsp;&emsp;&emsp;&emsp;30373E45,4C535A61,686F767D,848B9299,  
&emsp;&emsp;&emsp;&emsp;A0A7AEB5,BCC3CAD1,D8DFE6ED,F4FB0209,  
&emsp;&emsp;&emsp;&emsp;10171E25,2C333A41,484F565D,646B7279。  
&emsp;&emsp;&emsp;&emsp;解密密钥同加密密钥,解密使用的轮密钥由解密密钥生成,其轮密钥生成方法同加密过程的轮密钥生成方法。
