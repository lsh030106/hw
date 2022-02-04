# Win32 API preview



## MFC

> 마이크로소프트 사의 class library, Visual C++ 에 포함
>
> 클래스 라이브러리를 이용한 개발 방법은 API, OOP 에 대한 전문적인 부분 요구
>
> 전문 개발자용

## 변수 명명법과 윈도우즈 데이터 타입

+ Win programming prefix

  | 접두어 |      의미       |              국문               |
  | :----: | :-------------: | :-----------------------------: |
  |   cb   | Count of Bytes  |            바이트 수            |
  |   dw   |   double word   |       unsigned long type        |
  |   h    |     handle      | windows, bitmap, file 등의 핸들 |
  |   sz   | NULL Terminated |        NULL 종료 문자열         |
  |   w    |      Word       |        unsigned int type        |
  |   i    |     Integer     |               int               |
  |   b    |      Bool       |              bool               |

* windows.h data type

  | Data type |          의미           |
  | :-------: | :---------------------: |
  |   BYTE    |      unsigned char      |
  |   WORD    |     unsigned short      |
  |   DWORD   |      unsigned long      |
  |   LPSTR   |         char *          |
  |   BOOL    | int type, TRUE or FALSE |

  > 이렇게 새로운 데이터형을 선언해 사용하는 이유는 단순히 우리를 좆같게 하려고 하는게 아니다
  >
  > 윈도우즈가 항상 IBM 컴퓨터에서만 실행되리란 법은 없다
  >
  > 언젠가 다른 시스템으로 이식될 가능성이 있기에 소스 차원의 호환성을 확보하기 위해 사용하는 것

  예를 들어 WORD는 현재 2byte지만 언젠가 4byte로 바뀔수도 있다.

  그렇더라도 소스 수정 필요없이 헤더파일에서 WORD 형의 정의만 바꿔주고 컴파일 하면 

  호환성은 문제없다

## Windows 에서의 handle

> handle 이란 구체적인 대상에게 붙여진 번호, 32bit 의 정수값
>
> Dos 프로그래밍에서는 file handle 만이 사용되었지만 윈도우즈에서는 여러 종류의 핸들을 사용 중이다



### 핸들의 특성

* 핸들은 정수값이며 대부분의 경우 32bit다. 핸들을 사용하는 목적은 오로지 구분을 위한 것이니,

  핸들끼리 중복되지 않아야 한다.

  >

* 핸들은 OS가 발급해 주며 사용자가 사용한다. 윈도우를 만들거나 파일을 열면 OS는 만들어진 윈도우나 열려진 파일에 핸들을 붙여준다.

  사용자는 이 핸들을 보관해 두었다가 해당 윈도우나 파일을 참조할 때 핸들을 사용하면 된다

  사용자가 직접 핸들을 만드는 경우는 없다(kernel mode)

  >

* 같은 종류의 핸들끼리는 때려죽여도 절대 중복된 값을 가지지 않는다.

  핸들은 식별자다. 물론 뭐 다른 종류의 핸들끼리는 중복된 값을 가질 수 도 있다

  >

* 핸들은 정수형이니 값이 있지만 몰라도 상관없다.  숫자의 의미가 아니라 뭐랄까 그냥 해쉬값 같은 거다

  핸들형 변수(32bit 부호없는 정수)만들어서 쓰다 버려라



**윈도우에서 핸들은 예외없이 접두사 h로 시작되며 핸들값을 저장하기 위해 별도의 데이터형도 있다 찾아봐라**



## 비트 OR 연산자

함수 호출 과정에서 파라미터 전달시 Windows 는 윈도우의 스타일을 파라미터로 전달하는 데

예를 들어 CreateWindow 함수는 WS_CHILD, WS_CAPTION, WS_BORDER 등 옵션이 수십 개나 되지만

단 하나의 인수로 모든 파라미터를 전달받는다



Winuser.h 헤더를 보면

```c
/*
* DrawText() Format Flags
*/
#define DT_TOP		0x00000000
#define DT_LEFT		0x00000000
#defube DT_CENTER   0x00000001
.
.
.
#defube DT_INTERNAL 0x00001000
```

위의 값들을 | 연산 하면 함수는 파라미터의 개별 비트를 확인후 어떤 옵션들이 선택되었는지 알 수 있는 것

Windows API 에서 흔히 사용하는 인수 전달법이다