#pragma once


// CPage1 dialog

class CPage1 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPage1();

// Dialog Data
	enum { IDD = IDD_page1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
protected:
	afx_msg LRESULT OnUmMain(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmIdcheck(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton2();
};
