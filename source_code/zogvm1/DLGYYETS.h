#pragma once

#include "SqliteOperate.h"
// CDLGYYETS dialog

class CDLGYYETS : public CDialog
{
	DECLARE_DYNAMIC(CDLGYYETS)

public:
	CDLGYYETS(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGYYETS();

	BOOL OnInitDialog();
	BOOL	m_check;

	list<struct YYETS_ST> yyetsList;
	void DisplayerList();

	CRect printf_Rect;
	long ListTotal;
	long ListNow;
	long		m_page;
	CBitmapButton m_first_button;
	CBitmapButton m_pre_button;
	CBitmapButton m_next_button;
	CBitmapButton m_last_button;
	CBitmapButton m_jump_button;

	CString m_sreach;
	CListCtrl m_list;

	CSqliteOperate YyetsDB;
	// Dialog Data
	enum { IDD = IDD_YYETS };
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonMagnet();
	afx_msg void OnBnClickedButtonEd2k();
	afx_msg void OnBnClickedButtonWanpan();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnButtonFirst();
	afx_msg void OnButtonPrevious();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonLast();
	afx_msg void OnButtonJump();
	afx_msg void OnCheck();

};
