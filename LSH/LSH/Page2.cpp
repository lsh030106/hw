// Page2.cpp : implementation file
//

#include "stdafx.h"
#include "LSH.h"
#include "Page2.h"
#include "afxdialogex.h"

#define UM_main (WM_USER+2)

// CPage2 dialog

IMPLEMENT_DYNAMIC(CPage2, CDialogEx)

CPage2::CPage2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage2::IDD, pParent)
{

}

CPage2::~CPage2()
{
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPage2, CDialogEx)
	ON_MESSAGE(UM_main, &CPage2::OnUmMain)
	ON_BN_CLICKED(IDC_BUTTON1, &CPage2::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPage2 message handlers


afx_msg LRESULT CPage2::OnUmMain(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CPage2::OnBnClickedButton1()
{
	GetParent()->SendMessage(UM_main);
	// TODO: Add your control notification handler code here
}
