#pragma once
#include "afxcmn.h"


// CDLGxunleisub dialog

class CDLGxunleisub : public CDialog
{
	DECLARE_DYNAMIC(CDLGxunleisub)

public:
	CDLGxunleisub(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGxunleisub();

// Dialog Data
	enum { IDD = IDD_SUBLIST };
	BOOL OnInitDialog();
	int Choose;
	char path[260];
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFind();
	CString m_edit;
	CListCtrl m_list;
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
};
