// Idcheck.cpp : implementation file
//

#include "stdafx.h"
#include "LSH.h"
#include "Idcheck.h"
#include "afxdialogex.h"
#include "Userdb.h"

// CIdcheck dialog

IMPLEMENT_DYNAMIC(CIdcheck, CDialogEx)

CIdcheck::CIdcheck(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIdcheck::IDD, pParent)
{
	
}

CIdcheck::~CIdcheck()
{
}

void CIdcheck::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIdcheck, CDialogEx)
	ON_BN_CLICKED(IDOK, &CIdcheck::OnBnClickedOk)
END_MESSAGE_MAP()


// CIdcheck message handlers


void CIdcheck::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CUserdb userdb;
	userdb.id_dup();
}
