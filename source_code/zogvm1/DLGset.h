#pragma once

#include "SqliteOperate.h"
// DLGset dialog

class DLGset : public CDialog
{
	DECLARE_DYNAMIC(DLGset)

public:
	DLGset(CWnd* pParent = NULL);   // standard constructor
	virtual ~DLGset();

// Dialog Data
	enum { IDD = IDD_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual void OnCancel();
	BOOL OnInitDialog();
	BOOL m_c_net_proxy;
	CString m_e_net_proxy;
	BOOL m_c_sub_sv;
	BOOL m_c_svsub_onlyone;
	BOOL m_c_svsub_onlychn;
	BOOL m_c_mediainfo;
};
