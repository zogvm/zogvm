#pragma once

#include "SqliteOperate.h"
#include "afxwin.h"
// CDLGViewEd2k dialog
#define  MAX_EXTERN_ED2K_LIB_NUM 5

enum EXTERN_ED2K_LIB
{
	EXTERN_ED2K_NGC=0,
	EXTERN_ED2K_NHK=1,
	EXTERN_ED2K_MVG=2,
	EXTERN_ED2K_LYW=3,
	EXTERN_ED2K_DMHY=4,
};

class CDLGViewEd2k : public CDialog
{
	DECLARE_DYNAMIC(CDLGViewEd2k)

public:
	CDLGViewEd2k(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGViewEd2k();

// Dialog Data
	BOOL OnInitDialog();
	BOOL	m_check;

	list<struct EXTERN_ED2K_ST> ed2kList;
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

	void DisplaySize(long long size,char* str);

	char libpath[MAX_EXTERN_ED2K_LIB_NUM][520];
	char libname[MAX_EXTERN_ED2K_LIB_NUM][520];
	int combo_index[MAX_EXTERN_ED2K_LIB_NUM];
	int combo_total;
	
	CString m_sreach;
	CListCtrl m_list;

	CSqliteOperate ed2kDB;
	// Dialog Data
	enum { IDD = IDD_VIEW_ED2K };
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonEd2k();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnButtonFirst();
	afx_msg void OnButtonPrevious();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonLast();
	afx_msg void OnButtonJump();
	afx_msg void OnCheck();
	CComboBox m_c_lib;
	afx_msg void OnCbnCloseupComboLib();
};
