#pragma once

#include "SqliteOperate.h"
// CDLGAddEd2k dialog

enum ENCODE_EM
{
	ENCODE_ANSI=0,
	ENCODE_UTF8=1,
};

class CDLGAddEd2k : public CDialog
{
	DECLARE_DYNAMIC(CDLGAddEd2k)

public:
	CDLGAddEd2k(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGAddEd2k();
	BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	
	bool breakflag;
	
	list<struct EXTERN_ED2K_ST> ed2kList;


	CSqliteOperate INDB;
// Dialog Data
	enum { IDD = IDD_ADD_ED2K };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_e_txt;
	CString m_e_db;
	afx_msg void OnBnClickedButtonTxt();
	int m_c_encode;
};
