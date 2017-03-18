#pragma once


// CDLGMediaInfo dialog

class CDLGMediaInfo : public CDialog
{
	DECLARE_DYNAMIC(CDLGMediaInfo)

public:
	CDLGMediaInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGMediaInfo();

// Dialog Data
	enum { IDD = IDD_MEDIA_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();

	char path[520];
	CString m_edit;
	afx_msg void OnBnClickedButtonCopy();
};
