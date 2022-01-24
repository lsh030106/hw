
// LSHDlg.h : header file
//

#pragma once
#include "Main.h"
#include "Page1.h"
#include "Page2.h"
#include "Idcheck.h"

// CLSHDlg dialog
class CLSHDlg : public CDialogEx
{
// Construction
public:
	CLSHDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	enum { IDD = IDD_LSH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CMain m_main;
	CPage1 m_page1;
	CPage2 m_page2;
	CIdcheck m_idcheck;

protected:
	afx_msg LRESULT OnUmPage1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmMain(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmPage2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmIdcheck(WPARAM wParam, LPARAM lParam);
};
