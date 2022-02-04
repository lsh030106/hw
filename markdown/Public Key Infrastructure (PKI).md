## Public Key Infrastructure (PKI)

#### 공개 키 기반구조

  >Asymmetric cipher : 공개키/ 개인키 암호 시스템, 암호화 키와 복호화 키가 다르다

  * 기본적인 비대칭 키 암호화 통신 수행 과정
    * A 와 B 는 서로의 public key 을 교환한다. 공개키는 공개가 목적이다
    * A는 데이터를 B의 공개키로 암호화 해서 B에게 보낸다
    * B는 자신의 개인키로 데이터를 복호화 한다

---

 #### RSA

  >RSA는 완전한 PKI를 제공한다, 핵심은 공개키와 개인키를 만드는 키 생성 알고리즘

  * 소인수 분해의 어려움에 기반하여 공개키를 가지고 개인키를 유추하는 것이 어렵다.

  * 키 생성 과정에서 최초 선택되는 두 소수 (P, Q) 가 클 수록 공개키를 이용해 개인키 유추가 어렵다.

    

  * ##### RSA 키 생성 과정

  ```c
  /* 공식을 완벽히 구현하지 않아 많이 느리니 전체적인 흐름만 볼것
  modulus 와 오일러의 정리, 유클리드 호제법 등 공식 위키에서 찾아볼것
  간단히 법의 개념을 이용해 단방향으로는 값을 찾기 쉬우나, 역방향으로는 찾기 어려운 함수를 만든 것
  */
  
  #include <stdio.h>
  
  // 유클리드 호제법 (최대공약수를 구하는 알고s리즘)
  long gcd(long a, long b) {
     
      return b ? gcd(b, a % b) : a;
  }
  
  // 오일러 피 함수 (n의 서로소 개수 구하는 알고리즘)
  long gulerphi(long n) {
      
      long cnt = 0;
      for (long i = 1; i < n; i++) {
          
          if (gcd(n, i) == 1) {
              
              return cnt;
          }
      }
  }
  
  // 1보다 크고 Np보다 작은 서로소 정수 e 찾기
  long fgcd(long Np) {
  	
      for (long i = 2; i < Np; i++) {
          
          if (gcd(Np, i) == 1) {
              
              return i;
          }
      }
  }
  
  // 이거 확장 유클리드 알고리즘 아니다. 나중에 수정요망
  long eea(long e, long np) {
  	
      for (long i = 1; i < np; i++) {
          
          if ((i * e) % np == 1) {
              
              return i;
          }
      }
  }
  
  long mul(long a, long b) {
      
      return a * b;
  }
  
  long pulerphi(long P, long Q) {
      
      return (P - 1) * (Q - 1);
  }
  
  int main(void) {
      
      // 1. 서로 다른 두 소수를 선택한다
      long P = 47, Q = 71;
      
      // 2. P와 Q를 곱해 N을 구한다
      long N = mul(P, Q);
       
      // 3. 오일러 피 함수 Np 를 구한다
      long Np = pulerphi(P, Q);
  
      // 4. Np 보다는 작으면서 Np와 서로소인 정수 e를 찾는다
      long e = fgcd(Np);
      
      // 5. 확장된 유클리드 호제법을 이용해 (d * e)/Np 일 때 나머지가 1인 정수 d 를 구한다.
      long d = eea(e, Np);
      
      printf("PUBLIC KEY : [%ld, %ld]\n", N, e);
      printf("PRIVATE KEY : [%ld, %ld]\n", N, d);
      
      return 0;
  }
  ```

  	

   *  ##### 암호화

$$
  C = M^e\ mod\ N
$$

---

  * ##### 복호화

    $$
    M = C^d\ mod\ N
    $$

  * 

---

  

  * ##### 코드 구현

  

  ```c
  #include <stdio.h>
  #include <math.h>
  
  int encrypt(int M, int e, int N) {
  	
      return (int)pow(M, e) % N;
  }
  
  int decrypt(int C, int d, int N) {
      
      return (int)pow(C, d) % N;
  }
  ```