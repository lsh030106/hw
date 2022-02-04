# shadowWall 

> repository
>
> @TOP 파일로 TOP directory 를 찾는다. 
>
> top directory 의 build.sh 는 사용하지 않는 스크립트 
>
> .PHONY 는 dependency 가 없는 makefile 명령어로 더미 타겟을 명시해 해당 더미 타겟 규칙의 명령어를 수행하지 않는다
>
> _docker 가 붙은 directory 나 file은 신경 쓰지 않아도 된다. 필요없다
>
> orb는 Object Request Broker 로 distributed computing environment 에서 middle software 로 객체들 간 클라이언트/서버 관계를 맺어준다고 생각하면 된다
>
> dtd는 Document Type Definition 으로 XML, SGML, HTML 문서에 사용되는 요소와 속성을 지정하는 텍스트 파일, 유효한 문서인지 여부를 확인하는데도 사용



## docs 

> dita 파일로 저장되어 있는 설명서

* dita 란 DITA 형식으로 작성된 XML 파일로 기술 문서의 제작 및 게시에 사용됨



## sandbox 

> Test code를 모아놓은 곳



## bin, etc, include, lib, webdoc

> output directory, 

* bin : binary 파일을 출력받는다
* etc : configure 파일을 출력받는다
* include : header 파일들을 출력받는다
* lib : .a, .so 등 lib 를 출력받는다
* webdoc : front-end code, js, php 를 출력받는다



## rules

> base 가 되는 makefile (mk) 파일이 존재

* version.mk : bring 을 지정, kernel 의 version 을 지정
* common.mk : target 들을 정의한다. 1060 line 의 dump에서는 makefile의 변수를 확인할 수 있다



## bring

> shell(busybox), ipsec, ips 등 open source 설치
>
> netfilter -> libs -> app 순서

* make.common : 기본 설정

* Makefile 의 subsystems 을 확인하면 build 순서 확인가능

* 각 폴더의 make.common 의 KERNEL_DIR 이 output 경로

* patches

  * opensource 에서 bug가 있을 시 patch 후 build
  * diff -urN

* makefile 

  1. checkout : 다운 후 압축 풀기
  2. patch : patch 시 중복이 된다. make 시 distclean 필수
  3. build : configure, build/home/lsh/system_programming/minishell/src/main.c /home/lsh/system_programming/minishell/src/pipegrep.c /home/lsh/system_programming/minishell/src/test.xml

* netfilter

  * makefile.kernel : kernel build
  * makefile.libmnl : net-link build, net-link는 linux에서 kernel과 user space의 interface
  * makefile.libnf_ct : netfilter 의 connection tracking 
  * 등등 netfilter 의 linux kernel 단 network 관리 module 들을 build

  

* libs

  * openssl, hyperscan 등이 들어있음
  * makefile.hyperscan* : 수리카타에서 사용함
  * makefile.omniorb : 프로그램의 기반이 되는 library

* apps

  * makefile.spamassassin : perl 등을 이용해 build, dependency가 많아 문제가 있음



## IDL

> Interface Description Language 로 어느 한 언어에 국한되지 않는 언어중립적인 방법으로 인터페이스를 표현
>
> 함으로써, 같은 언어를 사용하지 않는 소프트웨어 컴포넌트 사이의 통신을 가능하게 함

* 대부분의 소스와 클래스, 변수들이 정의되어 있음
* 코드의 raise 는 예외처리



## kernel-modules

* dpi (deep packet inspection)

  * policy 중 dpi(심층 패킷 검사) 기능이 있음, 해당 기능은 message protocol을 보고 판별
  * 네트워크의 트래픽을 조정하기 위해 쓰이는 기술

* e100e, i40e, igb, r8168

  > NIC driver

  * e1000e, igb를 주로 사용
  * i40e 는 끝에 A가 붙는 장비가 사용하며 10GB 광랜을 지원한다

* geoip 는 국갈별 IP를 지원하나 유료화로 사용은 잘 하지 않는다

* qwkisscom

  * 유해정보 차단
  * r8168, bypass 와 더불어 build가 되지않아도 무방

* wcfilter 

  * web contents filter



## libsrc 

> build 시 오래 걸리며 shadowwall 의 핵심 프로그램을 설치한다
>
> dependency  때문에 순서가 매우 중요

tcore, rtnl, hwc, sysinfo 가 대표적으로 중요하다



* rtnl
  * network와 관련된 래퍼함수 (여기서는 C를 CPP 로 wrapping 을 시킨다)
* tcore
  * turtle core, base가 되는 코드
* sysinfo
  * tprconf 의 정보 및 dashboard 의 정보를 위한 코드
* xml++
  * xml 수정 lib를 cpp로 wrapping 한 것
* hwc 
  * 라이센스, 로그, 인증과 관련된 코드
  * license.cpp 와 login.cpp 가 중요, login.cpp는 리눅스 계정정보를 담는다 



## src



* 중요한 디렉토리
  * policy manager
  * sysmonitor
  * tools
  * webadmin
  * logstat
* webadmin
  * killhttpd
    * apache 시작 시 init의 기능을 하는 daemon
  * php-ominiorb
    * php, apache의 interface module 로 pm 과 연결하는 역할을 한다
  * resource
    * web 관련 코드들 
    * /application 
      * codeigniter framework 기반 back-end code
    * /app
      * Ext JS framework 기반 front-end code
    * 각각의 view 객체들은 build 가 지정되어 webdoc 에 output 된다
    * turtle.conf 에 압축되어 www 쪽에 압축이 풀린다
* policymanager
  * web 의 menu 대로 구분되어 있으며 web이랑 연동한다
  * 위는 전부 object oriented의  Factory pattern 의 개념이다
  * php-ominiorb가 불러 죽어도 될때까지 호출한다, 따라서 pm이 죽으면 web도 느리다
  * main
    * pm에 관한 소스코드
  * startup
    * main이 실행되며 하는 초기화 루틴
  * bootorb
  * killpm
    * bootorb 와 startup 을 설정하고 초기화하는 역할을 한다
    * 처음 부팅시 장비가 느린것이 killpm 때문이다
  * listeners
    * pm 이 설정되기 전 선행 process 들을 올려준다(기본 daemon 들을 띄어준다)
  * logstat 
    * log와 관련된 (syslog나 kernel log) 를 hooking하는 코드, opensource 는 syslog에 log를 남긴다
  * basesystem 
    * 네트워크 설정
  * config
    * 환경설정
  * firewallvpn
    * 긴급정책
  * object
    * 객체 관련(네트워크 객체, 사용자 객체)
  * sysinfo
    * dashboard 관련, interface 사용량  등
* sysmoniotr
  * daemon monitor, check integrity 등 system monitoring 관련 tool 들
  * daemon monitor
    * 필수 daemon 의 alive 여부를 확인
    * check 후 daemon 자고 있음 깨움 (daemon을 명시적으로 꺼도 계속 깨움)
* tools
  * tprconf, lcm 등 간혹 쓰이는 유용한 tool들을 모아놓음
* logstat
  * 조건이 되면 syslog 에서 hooking
  * rrd-scripts
    * traffic을 graph 로 표현 
  * ulogd
    * log를 출력가능한 프로그램을 별도의 파일에 띄우거나 db에 저장



## datasrc

> 설정 파일들 (config, turtle etc, postgresdb)
>
> turtle_etc 를 수정할시 디렉토리 내부 turtle.duruan.conf 를 수정한다

* turtle_etc : 기본 파일 정의
  * passwd, shadow 등은 대표적으로 rootfs 에 복사되는 파일
* xml
  * XML 수정 시 먼저 dtd 파일을 수정해야 한다
  * updateconfig.cml 은 규칙업데이트

* message 
  * 다국어 관련 / po 파일 사용 
* mlibs
  * snmp 에서 사용하는 파일 (OID를 정의하는 파일)
* mail
  * postfix solution 으로 mail을 발송한다





## 작용



1. web 에서 EXT JS 로 Request
2. codeigniter(CI) 에서 php-ominiORB로 PM Call (IDL를 이용한 형변환)
3. PM에서 return, 역방향으로 CI에서 EXT JS에 Response 

> 기능 추가, 수정, 삭제 시 EXT JS, Codeigniter, IDL, PHP-ominiORB, Policy manager 전부 수정 필요

web 쪽 권한은 HTTPD daemon 이 가지고 있다. 따라서 turtle etc 등 root 권한이 필요한 file 수정이 불가하다

PM은 root 권한을 가지고 있어 CI는 pm에게 요청을 통해 root 권한을 간접적으로 행사 할 수있다

CI는 대신 DB connection 을 통해 data 를 교환할 수 있다



CGI programming의 개념이 있어야한다 

JS , XML, DTD, IDL 의 문법이 숙달 되어야 한다



