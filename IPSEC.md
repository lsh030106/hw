## IPSEC

> * 보안에 취약한 구조를 가진 IP의 보안을 위해서 IETF 에서 설계한 표준(RFC2401)
>
> * Layer 3를 집중적으로 보안,  패킷에 대한 보안을 제공하는 IP Security





#### SP (Security Policy)

---

> ##### 일종의 패킷 필터링 정책
>
> + Incoming Traffic 과 Outgoing Traffic 으로 구분하여 정책을 지정
> + IP Packet의 Bypass, Discard, Protection 등을 지원
>   - Source/Desination IP / Port, 식별자
>   - TLP



#### SA (Security Associantion)

---

> IPSec 을 사용하는 두 Peer 간의 협약
>
> * 주요 요소
>   * Sequence Number Counter : Packet의 일련번호 카운터
>   * AH/ESP : Protocol 정보
>   * Life Time : 세션 만료 기간
>   * Mode : 동작 모드 (전송/ 터널 모드)
>   * Path MTU : 경로의 MTU 값



####  프로토콜

---

* ###### AH (Authentication Header)

  * IP Extension Header, IP Packet에 대한 인증을 제공

  * MAC(메세지 인증 코드) 를 이용해 Data Intergrity와 Authentication 기능 제공

  * 단, Payload 암호화는 제공하지 않는다 (비밀을 보장해 주지 않는다)

    

* ###### ESP (Encapsulating Security Payload)

  * 새로운 데이터 IP Packet을 만들고 기존 IP Packet을 Data Payload에 넣어 감싸는 방식

  * AH 가 가진 무결성과 인증도 제공, 추가적으로 대칭키 암호화를 통해 기밀성 제공

    

* ###### IKE (Internet key Exchange)

  * IPSec 에서 키 교환에 사용되는 프로토콜

  * UDP 500 Port 사용

    

#### 두가지 모드

---

##### 전송 모드(Transport Mode)

- 보호 범위

  * IP 패킷 전체를 보호하는 것이 아닌 IP 헤더를 제외한 IP 패킷의 Payload 만을 보호

    > Payload : 상위 층에서 보낸 데이터

     * IP 헤더는 암호화 X, 트래픽 경로는 노출된다

       > Traffic : 네트워크 장치에서 일정 시간 흐르는 데이터의 양

* 보호 구간

  * **host-to-host (end-to-end) 간 데이터 보호가 필요할 때 사용**

    > router 를 거쳐 두 당사자만 확인 가능 E2EP 가 이뤄진다



> *Transport Layer 와 Network Layer 사이에 IPSec Layer 가 추가된다고 생각하면 된다*

![](https://img1.daumcdn.net/thumb/R1280x0/?scode=mtistory2&fname=https%3A%2F%2Fblog.kakaocdn.net%2Fdn%2FeMGuHW%2FbtqFbBUnmK1%2FOeKLdM6nJuk5pOwOdcY2O1%2Fimg.png)



###### 전송 모드에서 AH

> 기존 IP Header 는 그대로 있고 기존 Payload 앞에 AH가 붙어서 새로운 Payload가 된다



###### 전송 모드에서 ESP 

> 기존 IP Header 는 그대로 있고 기존 Payload를 ESP로 감싼것이 새로운 Payload 가 된다
>
> Origin payload, ESP Trailer 에 대한 인증정보가 ESP Authentication 에 들어간다

---

##### 터널 모드 (Tunneling Mode)

* 보호 범위
  * IP 패킷 전체를 보호
  * 암호화된 IP 패킷에 IPSec 헤더를 추가해 라우팅을 수행
  * IPSec 헤더는 구간 간 이동에 대한 정보만 있으니 종단 정보(출발지, 목적지) 와 트래픽 경로는 보호



* 보호 구간
  * gateway-to-gateway, tunnel-to-gateway, end node-to-gateway 구간의 IP 패킷 보호



> Payload 만을 보호하느냐, 헤더까지 모두 보호하냐에 따라 각각 전송모드, 터널모드라고 함
>
> * header 새로만든다 -> Tennel



> IPSec 헤더와 새로운 IP 헤더는 호스트가 아닌 라우터가 추가한다

![](https://img1.daumcdn.net/thumb/R1280x0/?scode=mtistory2&fname=https%3A%2F%2Fblog.kakaocdn.net%2Fdn%2FdrY2z8%2FbtqFaTnUijV%2FOfShxsAuzQzdco711BExE0%2Fimg.png)



###### 터널모드에서 AH

> IP Header 가 새로 생성되고 기존 IP Packet 전체가 Payload가 된다



###### 터널 모드에서 ESP

> IP Header 가 새로 생성되고 기존 IP Packet 전체를 암호화하여 ESP 로 감싼 것이 새로운 Payload가 된다

