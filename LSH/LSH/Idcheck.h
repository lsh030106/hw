#pragma once


// CIdcheck dialog

class CIdcheck : public CDialogEx
{
	DECLARE_DYNAMIC(CIdcheck)

public:
	CIdcheck(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIdcheck();

// Dialog Data
	enum { IDD = IDD_IDCHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
