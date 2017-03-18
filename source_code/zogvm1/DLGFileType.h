#if !defined(AFX_DLGFILETYPE_H__112C8BA3_4870_49FC_8767_26E46BEC2840__INCLUDED_)
#define AFX_DLGFILETYPE_H__112C8BA3_4870_49FC_8767_26E46BEC2840__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLGFileType.h : header file
//
#include "SqliteOperate.h"
#include <vector>
#include <list>
using namespace::std;
/////////////////////////////////////////////////////////////////////////////
// CDLGFileType dialog

class CDLGFileType : public CDialog
{
// Construction
public:
	CDLGFileType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLGFileType)
	enum { IDD = IDD_FILETYPE };
	CListCtrl	m_list_other;
	CListCtrl	m_list_video;
	CListCtrl	m_list_sub;
	CListCtrl	m_list_music;
	BOOL	m_c_music;
	BOOL	m_c_sub;
	BOOL	m_c_video;
	BOOL	m_c_other;
	//}}AFX_DATA
	BOOL OnInitDialog();
	
	void InitVideoList(void);
	void DisplayVideoList(void);
	void InitMusicList(void);
	void DisplayMusicList(void);
	void InitSubList(void);
	void DisplaySubList(void);
	void InitOtherList(void);
	void DisplayOtherList(void);

	list<struct FILETYPE_ST> typeList;



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGFileType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLGFileType)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckVideo();
	afx_msg void OnCheckSub();
	afx_msg void OnCheckMusic();
	afx_msg void OnCheckOther();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonNew();
	CString m_e_new;
	int m_c_new;
	afx_msg void OnBnClickedButtonRebuild();
	BOOL m_c_noscan_video_min;
	long m_e_noscan_video_min;
	BOOL m_c_noscan_video_max;
	long m_e_noscan_video_max;
	BOOL m_c_noscan_audio_min;
	long m_e_noscan_audio_min;
	BOOL m_c_noscan_audio_max;
	long m_e_noscan_audio_max;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILETYPE_H__112C8BA3_4870_49FC_8767_26E46BEC2840__INCLUDED_)
