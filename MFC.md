# MFC

> 마이크로소프트 사에서 Application 만들라고 제공하는 C++ Class library, Virutal C++ 에 들어 있다.
>
> 파생되는 클래스들은 어떤 MFC를 기본 클래스로 두었느냐에 따라 규정된다

## AFX

* MFC Application program의 기본 골격을 AFX (Application Framework) class 라고 하는데,

  CWinApp, CDocument, CFrameWnd, CView 총 4개의 클래스로 구성된다.

* Test라는 이름의 프로젝트를 만든다면 일단 기본적으로 AFX class 를 **상속** 받은 파생 클래스들을 생성해주는 데 이 이름이 각각 CTestApp, CTestDoc, CMainFrame, CTestView 로 생성된다



+ ##### Main frame window class(CFrameWnd)

  윈도우 응용프로그램의 기본적인 틀 형태를 제공해주는 클래스로 제목줄, 메뉴, 툴바 등 여러 가지 자식 윈도우로 구성



* ##### Document Class (CDoocument)

  뷰에 출력될 데이터를 내부적으로 저장하고 관리하는 영역으로 우리가 볼 수는 없다



+ #####