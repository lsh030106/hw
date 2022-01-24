// Main.cpp : implementation file
//

#include "stdafx.h"
#include "LSH.h"
#include "Main.h"
#include "afxdialogex.h"
#include "Userdb.h"

#define UM_PAGE1 (WM_USER+1)
#define UM_page2 (WM_USER+3)

// CMain dialog

IMPLEMENT_DYNAMIC(CMain, CDialogEx)

CMain::CMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMain::IDD, pParent)
{
	
}

CMain::~CMain()
{
}

void CMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMain, CDialogEx)
	ON_MESSAGE(UM_PAGE1, &CMain::OnUmPage1)
	ON_BN_CLICKED(IDC_BUTTON1, &CMain::OnBnClickedButton1)
	ON_MESSAGE(UM_page2, &CMain::OnUmPage2)
	ON_BN_CLICKED(IDC_BUTTON2, &CMain::OnBnClickedButton2)
END_MESSAGE_MAP()



afx_msg LRESULT CMain::OnUmPage1(WPARAM wParam, LPARAM lParam)
{

	return 0;
}


void CMain::OnBnClickedButton1()
{
	GetParent()->SendMessage(UM_PAGE1);
	// TODO: Add your control notification handler code here
}


afx_msg LRESULT CMain::OnUmPage2(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CMain::OnBnClickedButton2()
{
	GetParent()->SendMessage(UM_page2);
	// TODO: Add your control notification handler code here
}
