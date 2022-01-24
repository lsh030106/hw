#pragma once


// CPage2 dialog

class CPage2 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage2)

public:
	CPage2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPage2();

// Dialog Data
	enum { IDD = IDD_page2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUmMain(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton1();
};
