# CSRF

>  Cross Site Request Forgery, 교차 사이트 요청 위조



* CSRF 공격이란 웹취약점 중 하나로, 이용자가 의도하지 않은 요청을 통한 공격을 의미함

​		공격자가 의도가한 행위를 특정 웹사이트에 요청하도록 만드는 공격

* 대표적으로 **GET /  POST** 를 이용한 공격이 있다.





### CSRF 방어

> 일반 사용자가 브라우저를 통해 처리할 수 있는 모든 요청에 대해 CSRF  보호를 사용하는 것을 권장

* 보통 CSRF 공격을 방어는 **조회(GET) 데이터는 방어 대상에 두지 않고 POST, PATCH, DELETE 메서드에만 적용**

#### Referrer 검증

* 서버단에서 request 의 referrer를 확인하여 domain 이 일치하는지 검증 하는 방법.
* referrer 검증만으로 대부분의 CSRF 공격을 방어할 수 있음
* Paros, Zap, fiddler 같은 프로그램으로 조작이 가능하기에 권장하지 않음



### Spring Security CSRF Token

* 임의의 토큰을 발급한 후 자원에 대한 변경 요청일 경우 Token 값을 확인 한 후 클라이언트가 정상적인 요청을 보낸것인 지 확인
* **만약 CSRF Token 이 존재하지 않거나, 기존의 Token 과 일치하지 않는 경우 4XX 상태코드를 리턴**

* 요청한 파라미터에 Token을 포함한 뒤 전송하여 정상적인 요청인지 혹은 공격자로부터 의도된 요청인지 판단할 수 있음







