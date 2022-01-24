#pragma once

// CMain dialog

class CMain : public CDialogEx
{
	DECLARE_DYNAMIC(CMain)

public:
	CMain(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMain();

// Dialog Data
	enum { IDD = IDD_main };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUmPage1(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton1();
protected:
	afx_msg LRESULT OnUmPage2(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton2();
};
