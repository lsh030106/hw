// Page1.cpp : implementation file
// sign up page

#include "stdafx.h"
#include "LSH.h"
#include "Page1.h"
#include "afxdialogex.h"

#define UM_main (WM_USER+2)
#define UM_idcheck (WM_USER+4)

// CPage1 dialog

IMPLEMENT_DYNAMIC(CPage1, CDialogEx)

CPage1::CPage1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPage1::IDD, pParent)
{

}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPage1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CPage1::OnBnClickedButton1)
	ON_MESSAGE(UM_main, &CPage1::OnUmMain)
	ON_MESSAGE(UM_idcheck, &CPage1::OnUmIdcheck)
	ON_BN_CLICKED(IDC_BUTTON2, &CPage1::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPage1 message handlers


void CPage1::OnBnClickedButton1()
{
	GetParent()->SendMessage(UM_main);
	// TODO: Add your control notification handler code here
}


afx_msg LRESULT CPage1::OnUmMain(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


afx_msg LRESULT CPage1::OnUmIdcheck(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CPage1::OnBnClickedButton2()
{
	GetParent()->SendMessage(UM_idcheck);
	// TODO: Add your control notification handler code here
}
