#pragma once

#include "SqliteOperate.h"
#include <vector>
#include <list>
using namespace::std;
// CDLGhdddelete dialog

class CDLGhdddelete : public CDialog
{
	DECLARE_DYNAMIC(CDLGhdddelete)

public:
	CDLGhdddelete(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGhdddelete();

// Dialog Data
	enum { IDD = IDD_HDD_DELETE };
	CListCtrl	m_list;
	BOOL	m_check;
	//}}AFX_DATA

	list<struct HDD_ST> hddList;

	BOOL OnInitDialog();
	void CDLGhdddelete::DisplayerList();
	// Overrides
	// ClassWizard generated virtual function overrides

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions

	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheck();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDeleteDouble();
	afx_msg void OnBnClickedButtonRebuildDouble();
};
