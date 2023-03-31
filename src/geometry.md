[toc]
# 三点确定椭圆

圆心为零点，三点投影到长短轴上，设长短轴矢量(a, b) (-b, a) ($a^2 + b^2=1$)

>$$\begin{cases}
(X_1, Y_1) = [(x_1,y_1) \cdot (a, b) , (x_1,y_1) \cdot (-b, a)]=(ax_1+by_1, -bx_1+ay_1) \\
(X_2, Y_2) = [(x_2,y_2) \cdot (a, b) , (x_2,y_2) \cdot (-b, a)]=(ax_2+by_2, -bx_2+ay_2) \\
(X_3, Y_3) = [(x_3,y_3) \cdot (a, b) , (x_3,y_3) \cdot (-b, a)]=(ax_3+by_3, -bx_3+ay_3)
\end{cases}$$

椭圆方程组
>$$\begin{cases}
X_1^2/M^2 + Y_1^2/N^2 = 1 \\
X_2^2/M^2 + Y_2^2/N^2 = 1 \\
X_3^2/M^2 + Y_3^2/N^2 = 1
\end{cases}$$

$M^2$和$N^2$不方便计算，设$m=1/M^2$, $n=1/N^2$ （m > 0, n > 0) 方程组展开
>$$\begin{cases}
m(x_1^2a^2+y_1^2b^2+2abx_1y_1) +n(x_1^2b^2 +y_1^2a^2-2abx_1y_1)=1  \quad(1)\\
m(x_2^2a^2+y_2^2b^2+2abx_2y_2) +n(x_2^2b^2 +y_2^2a^2-2abx_2y_2)=1  \quad(2)\\
m(x_3^2a^2+y_3^2b^2+2abx_3y_3) +n(x_3^2b^2 +y_3^2a^2-2abx_3y_3)=1  \quad(3)
\end{cases}$$

先消除ab: (1)式乘以$x_2y_2$ (2)式乘以$x_1y_1$ ($x_1y_1 \neq 0, x_2y_2 \neq 0$) 
相乘之后(2)式减(1)式 设$k=a^2$ 则 $b^2=1-k(k > 0)$
>$$k(m-n)\underline{[x_1y_1(x_2^2-y_2^2)-x_2y_2(x_1^2-y_1^2)]}+m\underline{(x_1y_1y_2^2-x_2y_2y_1^2)}+n\underline{(x_1y_1x_2^2-x_2y_2x_1^2)}=\underline{x_1y_1-x_2y_2}$$

将里面常量提取出来
>$$\begin{cases}
k_1&=x_1y_1(x_2^2-y_2^2)-x_2y_2(x_1^2-y_1^2) \\
m_1&=(x1_1y_1y_2^2-x_2y_2y_1^2) \\
n_1&=(x_1y_1x_2^2-x_2y_2x_1^2) \\
c_1&=x_1y_1-x_2y_2
\end{cases}$$

令$ m'=m-n$,则$m=m'+n \quad m'\text可以为负$
>$$\begin{cases}
km'k_1+(m'+n)m_1+nn_1=c_1 \\
km'k_1+m'm_1+n(m_1+n_1)=c_1
\end{cases}$$

令$n_1'=m_1+n_1$
>$$km'k_1+m'm_1+nn_1'=c_1 \quad(a)$$

(3)式得出结果
>$$km'k_2+m'm_2+nn_2'=c_2 \quad(b)$$

由(a)和(b)得出, 分别乘$k_1 k_2$：
>$$\begin{aligned}
n&=[\underline{(c_2k_1-c_1k_2)}-m'\underline{(m_2k_1-m_1k_2)}]/\underline{(n_2'k_1-n_1'k_2)} \\
&=s+tm'=\begin{cases}s=(c_2k_1-c_1k_2)/(n_2'k_1-n_1'k_2) \\ t=(m_1k_2-m_2k_1)/(n_2'k_1-n_1'k_2)\end{cases}
\end{aligned}$$


将上式代入第(a)式得到
> $$\begin{aligned}
k&=[\underline{c_1-sn_1'}-\underline{(tn_1'+m_1)m'}]/m'k_1 \\
&=o+p/m'=\begin{cases}o=-(tn_1'+m_1)/k_1\\ p=\quad(c_1-sn_1')/k_1\end{cases}
\end{aligned}$$

k、m、n 都表示成$m'$的方程式
>$$\begin{cases}
n&=s+tm'  \\
k&=o+p/m' \\
m&=n+m'
\end{cases} \quad (c)$$

代入原方程(1)
>$$\begin{aligned}
ab&=[1-(n+m')y_1^2-nx_1^2-km'(x_1^2-y_1^2)]/2x_1y_1m' \\
&=[1-n(x_1^2+y_1^2)-m'y_1^2-km'(x_1^2-y_1^2)]/2x_1y_1m'   \\
&=[1-(s+tm')(x_1^2+y_1^2)-m'y_1^2-(o+p/m')(x_1^2-y_1^2)]/2x_1y_1m' \\
 &=r/m'+q=\begin{cases}r=[1-s(x_1^2+y_1^2)-p(x_1^2-y_1^2)]/2x_1y_1 \\q=-[t(x_1^2+y_1^2)+y_1^2+o(x_1^2-y_1^2)]/2x_1y_1 \end{cases}
 \end{aligned}$$
 
 上式两边平方
 >$$k(1-k)=(ab)^2=(r/m'+q)^2$$

 得出关于$m'$的一元二次方程
 >$$Am'^2+Bm'+C=0\begin{cases}
 A=o-o^2-q^2       \\
 B=p-2op-2rq       \\
 C=-p^2-r^2
 \end{cases}$$

 判断$B^2-4AC>0$
 >$$m' = (-B \pm \sqrt{B^2-4AC})/2A$$

  结合(c)方程组，求出m>0、n>0、k>0。最终椭圆参数如下：
  >$$\begin{cases}
  M&=\sqrt{1/m}        \\
  N&=\sqrt{1/n}        \\
  a&=\sqrt{k}          \\      
  \end{cases}$$

以上化简是在第一步乘数不为零的情况下进行的，也是最麻烦情况下的推导，另外需要考虑$x_1y_1=0\quad x_2y_2=0\quad x_3y_3=0$
- 其中两者为0时，第三者满足则有解
- 其中一个为0时(直接化简得到$k_1,m_1,n_1,c_1$), 另外两方程合并得出$k_2,m_2,n_2,c_2$


一般情况代码
```C
float R2D(float angle) {
    return angle / 180 * 3.1415926f;
}

int main()
{
    int M = 20;
    int N = 10;
    float x_1(M*cos(R2D(30.0f))), y_1(N*sin(R2D(30.f)));
    float x_2(M*cos(R2D(60.0f))), y_2(N*sin(R2D(60.f)));
    float x_3(M*cos(R2D(120.0f))), y_3(N*sin(R2D(120.f)));

    float x_1_2 = pow(x_1, 2), y_1_2 = pow(y_1, 2);
    float x_2_2 = pow(x_2, 2), y_2_2 = pow(y_2, 2);
    float x_3_2 = pow(x_3, 2), y_3_2 = pow(y_3, 2);

    float k_1 = x_1*y_1*(x_2_2 - y_2_2) - x_2*y_2*(x_1_2 - y_1_2);
    float m_1 = x_1*y_1*y_2_2 - x_2*y_2*y_1_2;
    float n_1 = x_1*y_1*x_2_2 - x_2*y_2*x_1_2;
    float c_1 = x_1*y_1 - x_2*y_2;

    n_1 += m_1;

    float k_2 = x_1*y_1*(x_3_2 - y_3_2) - x_3*y_3*(x_1_2 - y_1_2);
    float m_2 = x_1*y_1*y_3_2 - x_3*y_3*y_1_2;
    float n_2 = x_1*y_1*x_3_2 - x_3*y_3*x_1_2;
    float c_2 = x_1*y_1 - x_3*y_3;

    n_2 += m_2;

    float s = (c_2*k_1 - c_1*k_2) / (n_2*k_1 - n_1*k_2);
    float t = (m_1*k_2 - m_2*k_1) / (n_2*k_1 - n_1*k_2);

    float o = -(t*n_1 + m_1) / k_1;
    float p = (c_1 - s*n_1) / k_1 ;

    float r = (1 - s*(x_1_2 + y_1_2) - p*(x_1_2 - y_1_2)) / (2 * x_1 * y_1);
    float q = -(t*(x_1_2+y_1_2) + y_1_2 + o*(x_1_2-y_1_2)) / (2 * x_1 * y_1);

    float A = o - o*o - q*q;
    float B = p - 2 * o*p - 2 * r*q;
    float C = -p*p - r*r;

    float m_ = (-B - sqrt(B*B-4*A*C)) / (2 * A);

    float n = s + t * m_;
    float k = o + p / m_;
    float m = n + m_;

    float M_1 = sqrt(1 / m);
    float N_1 = sqrt(1 / n);
    float A_1 = sqrt(k);

    return 0;
}
```

# 点到椭圆切线
定义：点(x,y) 椭圆上切点$(acos\theta, bsin\theta)$ 切点斜率$(-asin\theta, bcos\theta)$
由于点和切点边线应该与斜率一致，则：
>$$\begin{cases}
(x,y) -(acos\theta, bsin\theta) &= k(-asin\theta, bcos\theta) \\
(x-acos\theta, y-bsin\theta)&=k(-asin\theta, bcos\theta)  \\
(x-acos\theta)/(-asin\theta) &=(y-bsin\theta)/bcos\theta=k  \\
ab-xbcos\theta-aysin\theta&=0  \\
\end{cases}$$

令$t=cos\theta$
>$$ab-xbt=\pm \sqrt{1-t^2}ay$$

两边同时平方合并
>$$[(xb)^2+(ay)^2]t^2-2abxbt+(ab)^2-(ay)^2=0$$

# 点到椭圆垂足
## 线代解法
定义: 点(x,y) 椭圆上垂足$(acos\theta, bsin\theta)$ 垂足切线向量$(-asin\theta, bcos\theta)$
由于点到垂足向量点乘斜率应为0
>$$\begin{aligned}
&\Rightarrow ((x,y)-(acos\theta, bsin\theta))\cdot (-asin\theta, bcos\theta)=0 \\
&\Rightarrow xasin\theta-ybcos\theta-(a^2-b^2)sin\theta cos\theta=0
\end{aligned}$$

令$k=sin\theta$
>$$\begin{aligned}
&\Rightarrow xak-yb\sqrt{1-k^2}-(a^2-b^2)k\sqrt{1-k^2}=0     \\
&\Rightarrow xak=(yb+(a^2-b^2)k)\sqrt{1-k^2}       \\
&\Rightarrow mk=(n+pk)\sqrt{1-k^2}=\begin{cases}m=xa\\n=yb\\p=a^2-b^2 \end{cases}
\end{aligned}$$

两边同时平方
>$$\begin{aligned}
&\Rightarrow (mk)^2=(1-k^2)[n^2+2npk+(pk)^2] \\
&\Rightarrow p^2\underline{k^4}+2np\underline{k^3}+(m^2+n^2-p^2)\underline{k^2}-2np\underline{k}-n^2=0
\end{aligned}$$

这是一个一元四次方程，直接用求根公式求解
## 拉格朗日解法 
定义：点(a,b) 椭圆上一点(x,y)
>$$\begin{cases}
L(x,y) = (x-a)^2+(y-b)^2+\lambda (mx^2+ny^2-1) \\
L_x=x-a+\lambda mx=0 \Rightarrow x=a/(1+\lambda m)  &(1)\\
L_y=y-b+\lambda ny=0 \Rightarrow y=b/(1+\lambda n)  &(2)\\
mx^2+ny^2=1                                         &(3)
\end{cases}$$

将(1)和(2)代入(3)式
>$$
m(\frac{a}{1+\lambda m})^2+n(\frac{b}{1+\lambda n})^2=1 \Rightarrow
ma^2(1+\lambda n)^2+nb^2(1+\lambda m)^2=(1+\lambda m)^2(1+\lambda n)^2
$$
$$
m^2n^2\lambda^4+2(mn^2+nm^2)\lambda^3+(4mn+n^2+m^2-ma^2n^2-nb^2m^2)\lambda^2+2(m+n-mnb^2-mna^2)\lambda + (1-ma^2-nb^2) = 0
$$

解出$\lambda$并代入(1)式和(2)式

# 三圆的共切圆
已知三圆参数$(x_1, y_1, r_1),(x_2,y_2,r_2),(x_3,y_3,r_3)$和切圆参数$(x, y, r)$
>$$\begin{cases}
(x-x_1)^2+(y-y_1)^2=(r\pm r_1)^2  &(1)\\
(x-x_2)^2+(y-y_2)^2=(r\pm r_2)^2  &(2)\\
(x-x_3)^2+(y-y_3)^2=(r\pm r_3)^2  &(3)\\
\end{cases}$$

将(2)-(1)、(3)-(1)式得出方程组(上式以-号处理)
>$$\begin{cases}
\underline{2(x_1-x_2)}x+\underline{2(y_1-y_2)}y+\underline{2(r_2-r_1)}r+\underline{[(x_2^2+y_2^2-r_2^2)-(x_1^2+y_1^2-r_1^2)]}=0 \\
\underline{2(x_1-x_3)}x+\underline{2(y_1-y_3)}y+\underline{2(r_3-r_1)}r+\underline{[(x_3^2+y_3^2-r_3^2)-(x_1^2+y_1^2-r_1^2)]}=0
\end{cases}$$

按照$ax+by+cr+d=0$简化上式得到系数如下
>$$\Rightarrow (1)=\begin{cases}
a_1=2(x_1-x_2)\\b_1=2(y_1-y_2)\\c_1=2(r_2-r_1)\\d_1=(x_2^2+y_2^2-r_2^2)-(x_1^2+y_1^2-r_1^2)
\end{cases}
(2)=\begin{cases}
a_2=2(x_1-x_3)\\b_2=2(y_1-y_3)\\c_2=2(r_3-r_1)\\d_2=(x_3^2+y_3^2-r_3^2)-(x_1^2+y_1^2-r_1^2)
\end{cases} $$

>$$\Rightarrow\begin{cases}
a_1x+b_1y+c_1r+d_1=0 \\
a_2x+b_2y+c_2r+d_2=0
\end{cases}
\Rightarrow\begin{cases}
x=mr+n \\
y=mr+n
\end{cases}$$

将x, y代入方程组(1)得出关于r的方程
# 最小公倍数
已知两个数a, b， 其最小公倍数为am=bn(m,n互质) 以下是正面求解的推导证明过程：
设$b=ka+c(k=b/a\quad c=b\div a)$
>$$\begin{aligned}
\underline{a}m&=\underline{b}n &(1) \quad a < b\\
\Rightarrow am&=(ka+c)n\\
\Rightarrow \underline{c}\ n&=\underline{a}(m-kn) &(2) \quad c < a
\end{aligned}$$

**推导**：观察(1)式和(2)式
- 求解a,b的最小公倍数转化成求解c,a的最小公倍数
- c,a的最小公倍数除以c等于n再乘以b即为a,b的最小公倍数

**证明**：(2)式n与(1)式n是否相等？
     $\because m与n互质，则n与m-kn也互质
     \therefore (2)式n与(1)式n相等$

```c
int LCM(a, b) {
    int  c = b % a;
    if (c == 0)
       return b;

    return LCM(c, a) / c * b;
}
```

# 牛顿迭代法解切线和垂足
## 切线
同向量叉乘为0
>$$\begin{cases}
F(x)=f'(x)\times  (f(x) - P) = 0  &(1)\\
x_{n+1}=x_n-\frac{f(x_n)}{f'(x_n)}  &(2)
\end{cases}$$
>$$\begin{aligned}
\Rightarrow \frac{F(x)}{F'(x)} = \frac{f'(x)\times (f(x)-P)}{(f'(x)\times (f(x)-P))'} = \frac{f'(x)\times (f(x)-P)}{f''(x)\times  (f(x)-P) + f'(x)\times  f'(x)} \\
\Rightarrow \frac{V\times  V_1}{V\times  V_2 + V_1 \times  V_1} = \frac{V\times  V_1}{V\times  V_2}(V=f(x)-P,V_1=f'(x),V_2=f''(x))
\end{aligned}$$

## 垂足
>$$\begin{cases}
F(x)=f'(x)\cdot (f(x) - P) = 0  &(1)\\
x_{n+1}=x_n-\frac{f(x_n)}{f'(x_n)}  &(2)
\end{cases}$$
>$$\begin{aligned}
\Rightarrow \frac{F(x)}{F'(x)} = \frac{f'(x)\cdot(f(x)-P)}{(f'(x)\cdot(f(x)-P))'} = \frac{f'(x)\cdot(f(x)-P)}{f''(x)\cdot (f(x)-P) + f'(x)\cdot f'(x)} \\
\Rightarrow \frac{V\cdot V_1}{V\cdot V_2 + V_1 \cdot V_1}(V=f(x)-P,V_1=f'(x),V_2=f''(x))
\end{aligned}$$


