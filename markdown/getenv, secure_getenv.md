# getenv, secure_getenv

> #include <stdlib.h>
>
> char *getenv(const char *name);
>
> char *secure_getenv(const char *name);

+ getenv() 함수는 환경 목록을 검색해 환경 변수 이름을 찾고 해당하는 문자열 값에 대한 포인터를 반환

+ secure_getenv() 함수는 "secure execution"이 필요한 경우 NULL 값을 반환하는 것을 제외하고 **getenv()** 함수와 같다. 

  호출 프로세스에서 실행한 프로그램이 로드되었을 때 아래 조건 중 하나가 참이 경우 secure execution이 필요

  * 프로세스의 유효 사용자 ID 가 실제 사용자 ID와 일치하지 않거나 프로세스의 유효 그룹 ID가 실제 그룹 ID 와 일치 하지 않을 때

  * 실행 파일에 유효 기능 비트(effective capability bit) 가  set 되었을 때

  * 프로세스의 허용 기능 집합이 비어 있지 않을 때

    > the process has a nonempty permitted capability set

+ 일부 linux security modules 에 의해 트리거 된 경우에도 secure execution 이 필요할 수 있다

+ secure_getenv() 함수는 set-user-ID 또는 set-group-ID 프로그램이 환경을 실수로 신뢰할 경우 발생할 수 있는 취약성을 방지하기 위해 범용 라이브러리에서 사용하도록 설계



+ The getenv() function returns a pointer to the value in the environment, or NULL if there is no match.



+ Thread safety (MT -Safe env)



+ environment list의 문자열은 name=value 형식

  secure_getenv()의 secure execution 모드는 커널에서 사용자 공간으로 전달되는 보조 벡터에 포함된 AT_SECURE 플래그에 의해 제어됩니다.