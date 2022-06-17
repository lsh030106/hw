# C++ Google Unit Test

> Google Test Framework 로 Google의 특정 요구사항 및 제액사항을 기반으로 테스트 기술팀에서 개발한 Testing 프레임 워크



### 요구 사항

* **운영체제**
  - Linux
  - macOS
  - Windows
* **컴파일러**
  * gcc 5.0+
  * clang 5.0+
  * MSVC 2015+
  * mac OS 에서 Xcode 9.3+ 은  clang 5.0+ 



> GTEST 빌드는 공식 가이드 참조
>
> https://google.github.io/googletest/quickstart-cmake.html



### Google Test 란



* 독립적이고 반복 가능한 테스트
  - 서로 다른 object 에서 테스트를 각각 실행하여 테스트를 격리하며 테스트가 실패시 별도로 디버깅이 가능
* 조직화되며 테스트되는 코드의 구조를 반영
  * gtest 는 서로 연관된 테스트들을 데이터와 서브루틴을 공유하고 test suite로 그룹화 시킬 수 있다

- 이식과 재사용이 용이하다

  - 플랫폼 중립적인 코드로 많은 컴파일러, OS에서 작동시킬 수 있어 다양한 구성에서 테스트가 가능하다

- 최대한 많은 정보 제공한다

  - gtest는 첫번째 테스트 실패에서 중지하지 않고 다음 테스트를 계속하기 때문에 run-edit-compile cycle 에서

    여러 버글를 감지하고 수정이 가능하다

- 사용자가 테스트에 집중 할수 있다

  - gtest는 자동으로 테스트를 추적해 사용자가 테스트를 실행하기 위해 테스트를 일일히 열거하지 않아도 된다

- 테스트는 빨라야한다

  - gtest를 사용 시 테스트 간에 공유 자원을 재사용할 수 있으며(reuse shared resources) 테스트를 서로 종속시키지 않고 설정/해제 (set-up/tear-down) 비용을 한번만 지불할 수 있다



### Assertions

+ gtest assertion은 함수 호출과 유사한 매크로,**동작에 대한 assertion 을 만들어 클래스 또는 함수를 테스트 한다**
+ assertion 이 실패하면 gtest는 실패 메시지와 함께 assertion의 소스파일과 line 위치를 print 하며 사용자 정의 메시지를 제공가능하다



|   ASSERT_*   |    Fatal failures     | 현재 함수를 종료(abort the current function) |
| :----------: | :-------------------: | :------------------------------------------: |
| **EXPECT_*** | **Nonfatal failures** |       **현재 함수를 종료 시키지 않음**       |

* custom failure message 
  * << operator 로 정의 가능

```c++
#ex

ASSERT_EQ(x.size(), y.size()) << "여기에 사용자정의 실패 메시지 입력";

for (int i = 0; i < x.size(); ++i) {
  EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
}
```

> ostream assertion macro 로 스트리밍 되며  C String 및 string object 로도 스트리밍 될 수 있다.



**기본 true/false 상태를 테스트**

|     Fatal assertion      |    Nonfatal assertion    |      Verifies      |
| :----------------------: | :----------------------: | :----------------: |
| ASSERT_TRUE(condition);  | EXPECT_TRUE(condition);  | condition is true  |
| ASSERT_FALSE(condition); | EXPECT_FALSE(condition); | condition is false |



**두 개의 값을 비교**

> 비교 연산자를 사용할 수 없을 시 컴파일 오류가 발생

|    Fatal assertion     |   Nonfatal assertion   |   Verifies   |
| :--------------------: | :--------------------: | :----------: |
| ASSERT_EQ(val1, val2); | EXPECT_EQ(val1, val2); | val1 == val2 |
| ASSERT_NE(val1, val2); | EXPECT_NE(val1, val2); | val1 != val2 |
| ASSERT_LT(val1, val2); | EXPECT_LT(val1, val2); | val1 < val2  |
| ASSERT_LE(val1, val2); | EXPECT_LE(val1, val2); | val1 <= val2 |
| ASSERT_GT(val1, val2); | EXPECT_GT(val1, val2); | val1 > val2  |
| ASSERT_GE(val1, val2); | EXPECT_GE(val1, val2); | val1 >= val2 |



**양 쪽 CString 을 비교**

> string 객체의 비교를 원한다면 EXPECT_EQ, EXPECT_NE 등을 사용
>
> CASE가 들어갈 시 대소문자를 무시한다는 의미
>
> NULL 포인터와 빈 string 은 다르다고 판단함

|        Fatal assertion        |      Nonfatal assertion       |              Verifies              |
| :---------------------------: | :---------------------------: | :--------------------------------: |
|   ASSERT_STREQ(str1, str2);   |   EXPECT_STREQ(str1, str2);   |            same content            |
|   ASSERT_STRNE(str1, str2);   |   EXPECT_STRNE(str1, str2);   |         different content          |
| ASSERT_STRCASEEQ(str1, str2); | EXPECT_STRCASEEQ(str1, str2); |   same content<br />ignore case    |
| ASSERT_STRCASENE(str1, str2); | EXPECT_STRCASENE(str1, str2); | different content<br />ignore case |





### 테스트 예시

1. 메크로를 사용해 테스트 기능을 정의하고 이름을 지정, 값을 반환하지 않는 일반적인 C++ 함수

2. 이 함수에서 포함하려는 C++ 문과 다양한 gtest assertion 을 사용해 값을 확인

3. 테스트 결과는 assertion 에 의해 결정된다. 위의 assertion 확인



```c++
// test suite 매크로 함수 선언

TEST(TestSuiteName, TestName) {
    ... test body ...
}
```



* TEST() 의 argument 는 Test Suite name 과 test suite 내부 test 이름이다.
* 두 이름 모두 유효한 식별자여야 하며 밑줄(_)을 포함해서는 안된다.
* 테스트의 전체 이름은Test Suite Name 과 Test Name 으로 구성되며 Test Suite 의 Test 는 동일한 이름을 가질 수 있다. 



```c++
// test 하고자 하는 함수
// 인자 n 의 팩토리얼을 리턴함
int Factorial(int n); 
```



위 함수의 test suite 는

```c++
// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(Factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}
```



**gtest 는 테스트 결과를 테스트 suite 별로 그룹화하므로 논리으로 관련된 테스트는 동일한 테스트 모음에 있어야 한다**

* TEST() 매크로의 첫 번째 인자인 Test suite name 를 동일하게 맞춰 그룹화함
* Test suite 와 Test 이름을 지정할 때는 함수 및 클래스의 이름을 지정할 때와 동일한 규칙을 따른다



상기 내용은 Linux, Windows, Mac 모두 동일하다.



### 여러 테스트에 동일한 데이터 구성 사용

> Test Fixtures



* Test Fixtures 란 소프트웨어를 지속적으로 테스트하는 데 사용되는 **환경** 





