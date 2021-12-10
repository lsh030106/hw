## Linux server Core dump

> 리눅스 서버에서는 기본적으로 dump 파일이 생성되지 않는다



* ##### Core dump 생성 준비

  > https://www.kernel.org/doc/Documentation/sysctl/kernel.txt
  >
  > 해당 사이트에서 core_pattern, core_pipe_limit, core_uses_pid 만 검색해서 읽어보자
  >
  >  
  >
  > * core_pattern 
  >
  >   * core dump file pattern 이름을 지정하는데 사용한다고 한다, default  name은 core<br>최대길이는 127자다. 패턴 템플렛으로 사용되며 %문자열 패턴은 실제 값을로 대체된다
  >
  >   * backward compatibility with core_uses_pid:<br>    core_pattern에 "%p" 가 포함되지 않고 (기본값은 포함되지 않음) core_use_pid<br>    0이 아닌 값으로 설정된 경우 .PID 가 된다
  >
  >   * 첫번째 패턴 문자가 '|' 인 경우 명령어가 실행된다
  >
  >     
  >
  > * core_pipe_limit
  >
  >   * 해당 sysctl 은 core_pattern이 splace helper로 pipe 되도록 구성된 경우에만 적용된다
  >   * 병렬로 pipe 될 수있는 프로세스 수를 정하는 것. default는 0으로 제한하지 않는다
  >
  > * core_uses_pid 
  >   * 기본 코어 덤프파일 이름은 core 지만 core_uses_pid 를 1 로 설정하면 coredump <br>파일 이름이 core.PID 가 된다 (core_pattern에 "%p" 가 포함 되면 안된다)

  ---

  

  ```bash
  위 내용과 같으며 실제로 core_pattern에 들어가보면 
  |/usr/share/apport/apport %p ... 이렇게 적혀있는 파일이 존재한다
  
  해당 위치에는 파이썬으로 작성된 것으로 보이는 apport 소스파일이 존재하고 인터프리터 언어이니
  코드를 읽어보는 것도 가능하다
  ```

  ###### 이제 core 파일이 생성되도록 설정해보자

  

  ```bash
  # ulimit 는 프로세스의 자원의 한도를 설정하는 명령, soft 한도와 hard 한도로 나뉜다
  
  ulimit -a     # 소프트 설정 보기 : 새로운 프로그램을 생성하면 기본으로 적용
  ulimit -aH	  # 하드 설정 보기 : 소프트한도에서 최대로 늘릴 수 있는 한도
  
  ulimit -a	
  ```

  ```bash
  core file size	(block, -c)0
  # 기본적으로 코어 파일의 사이즈는 0임을 확인할 수 있다
  
  core -c unlimited 
  # core file size의 제한을 푼다. 
  ```

  하지만 위 방법은 일시적으로 재부팅이나 초기화를 하면 설정값으로 다시 바뀐다

  영구적으로 core file size를 설정하고 싶다면

  ```bash
  cd /etc/security/limits.conf
  
  #*		soft	core	0
  
  # 해당 부분의 주석을 지우고 원하는 값을 넣어준다
  
  *		soft	core 	unlimited
  ```

  이제 프로세스를 의도적으로 죽이거나 코드에서 의도적으로 오류를 발생시켜 core dump를 일으키면

  

  ```bash
  ls -a
  # 아래와 같은 파일이 생성될 것이다
  core
  ```

  위에서 봤듯 core_pattern 을 만지면 이름을 변경할 수 있다고 하나 apport 가 안되는 경우가 많아

  여러 솔루션들이 systemd-coredump 를 이용하라거나 apport를 종료하라고 한다

  ```bash
  # 시간적 여유가 없어 core dump file 이 core.PID 를 가지게만 만들겠다
  
  # 직접적으로 경로에 들어가 파일을 수정해도 괜찮으나 커널 파라미터를 수정하는 게 편하다
  
  # 이름은 core로
  sudo sysctl -w kernel.core_pattern=core
  # core.PID 형태를 가지게 한다
  sudo sysctl -w kernel.core_uses_pid=1
  
  # 적용이 되었나 확인
  sudo sysctl -a | grep core
  ```

   

  만약 영구적으로 파라미터를 수정하고 싶다면 /etc/sysctl.conf 에서 수정 할 수있다

  ```bash
  # root 계정이라면 이렇게 간편하게 수정할 수 있다
  # 내가 알기론 수정이 아니라 밑에 쓰는거라 다시 0으로 변경할 때는 잘 모르겠다
  acho "kernel.core_uses_pid=1" >> /etc/sysctl.conf
  ```

  /proc/sys/kernel/core_... 경로에서 바꾸는 것도 내 환경에선 문제 없었다

   ubuntu 18.04(5.4.0-90-g), 16.04(4.4.0-193-g)

  

  만약 생성이 되지 않는다면 

  ```bash
  sudo systemctl status apport
  
  # 해당 시스템 데몬이 실행중이라면 끄고 다시 시도
  
  sudo systemctl stop apport
  ```

  ---

* ##### Core dump file

  

  ```bash
  # 생성된 core dump file은 elf 형식이다. 그 말은 readelf 로 볼 수 있다는 것
  readelf -a core.5465
  
  # 우리가 원하는 메모리 스냅샷은 프로그램 헤더와 섹션에 있다는 걸 알 수 있다
  readelf -l core.5465
  ```

  이렇게 보면 스냅샷을 확인 할 수 있다.

  이제 gdb를 이용해 디버그를 해보는 방법이다

   

  ```bash
  gdb a.out core.5465
  #0  0x000055afd5eb0664 in devi ()
  
  #이처럼 gcc 할 때 -g 옵션을 주지 않았을 때는 함수이름만을 심볼로 가지고 있어 알려준다
  
  back trace 옵션을 사용할 수도 있고 각 스택 프레임을 f나 info f 옵션을 줘서 상세하게 확인도
  가능하다
  
  call stack을 추적하는 기능이 함수 이름과 함께 제공되며 info r 로 레지스터값도 확인 가능하다
  하지만 print 로 변수명은 심볼이 없기에 볼 수 없으며 함수명은 위에 나왔듯 찍어봐서 주소를 알 수 있다
  ```

  위처럼 컴파일 당시 gdb 옵션을 주지 않았음에도 back trace와 stack frame을 추적 할 수 있다

  
  
  
  
  ```bash
  # 컴파일 당시 gdb 옵션을 주면 오류가 났던 소스코드를 상세 정보와 함께 바로 보여주며 디버그를 수월하게 진행할 수 있게 core dump file이 도와준다 
  
  # 이번에 새로 알게된 gdb 기능인 특정 시점의 core dump를 생성하는 법이다
  generate-core-file 파일이름
  
  # 해당 시점의 덤프파일을 생성해 gdb로 분석할 수 있다
  ```
  
  

