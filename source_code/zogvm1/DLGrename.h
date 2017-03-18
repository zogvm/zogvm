#pragma once


// CDLGrename dialog

class CDLGrename : public CDialog
{
	DECLARE_DYNAMIC(CDLGrename)

public:
	CDLGrename(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGrename();

		char oldpath[520];
		char onlypath[520];
		char onlyname[520];
		char newpath[520];
		int re;
		BOOL CDLGrename::OnInitDialog();
// Dialog Data
	enum { IDD = IDD_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();


	CString m_newname;
};
