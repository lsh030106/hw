
// LSHDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LSH.h"
#include "LSHDlg.h"
#include "afxdialogex.h"

#define UM_page1 (WM_USER+1)
#define UM_main (WM_USER+2)
#define UM_page2 (WM_USER+3)
#define UM_idcheck (WM_USER+4)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLSHDlg dialog



CLSHDlg::CLSHDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLSHDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLSHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLSHDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_page1, &CLSHDlg::OnUmPage1)
	ON_MESSAGE(UM_main, &CLSHDlg::OnUmMain)
	ON_MESSAGE(UM_page2, &CLSHDlg::OnUmPage2)
	ON_MESSAGE(UM_idcheck, &CLSHDlg::OnUmIdcheck)
END_MESSAGE_MAP()


// CLSHDlg message handlers

BOOL CLSHDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect Frame;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&Frame);
	ScreenToClient(&Frame);
	
	m_main.Create(IDD_main, this);
	m_main.SetWindowPos(NULL, NULL, NULL, Frame.right, Frame.bottom, SWP_NOZORDER);
	m_main.ShowWindow(SW_SHOW);

	m_page1.Create(IDD_page1, this);
	m_page1.SetWindowPos(NULL, NULL, NULL, Frame.right, Frame.bottom, SWP_NOZORDER);
	
	m_page2.Create(IDD_page2, this);
	m_page2.SetWindowPos(NULL, NULL, NULL, Frame.right, Frame.bottom, SWP_NOZORDER);

	m_idcheck.Create(IDD_IDCHECK, this);
	m_idcheck.SetWindowPos(NULL, Frame.right, Frame.bottom, Frame.right, Frame.bottom, SWP_NOZORDER);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLSHDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLSHDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLSHDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CLSHDlg::OnUmPage1(WPARAM wParam, LPARAM lParam)
{
	m_main.ShowWindow(SW_HIDE);
	m_page1.ShowWindow(SW_SHOW);
	return 0;
}


afx_msg LRESULT CLSHDlg::OnUmMain(WPARAM wParam, LPARAM lParam)
{
	m_page1.ShowWindow(SW_HIDE);
	m_page2.ShowWindow(SW_HIDE);
	m_main.ShowWindow(SW_SHOW);

	return 0;
}


afx_msg LRESULT CLSHDlg::OnUmPage2(WPARAM wParam, LPARAM lParam)
{
	m_main.ShowWindow(SW_HIDE);
	m_page2.ShowWindow(SW_SHOW);
	return 0;
}


afx_msg LRESULT CLSHDlg::OnUmIdcheck(WPARAM wParam, LPARAM lParam)
{
	m_idcheck.ShowWindow(SW_SHOW);
	return 0;
}
