// DLGMediaInfo.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGMediaInfo.h"

#ifdef MEDIAINFO_LIBRARY
#include "MediaInfo/MediaInfo.h" //Staticly-loaded library (.lib or .a or .so)
#define MediaInfoNameSpace MediaInfoLib;
#else //MEDIAINFO_LIBRARY
#include "mediainfo_include/MediaInfoDLL.h" //Dynamicly-loaded library (.dll or .so)
#define MediaInfoNameSpace MediaInfoDLL;
#endif //MEDIAINFO_LIBRARY
#include <iostream>
#include <iomanip>
using namespace MediaInfoNameSpace;

// CDLGMediaInfo dialog

IMPLEMENT_DYNAMIC(CDLGMediaInfo, CDialog)

CDLGMediaInfo::CDLGMediaInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGMediaInfo::IDD, pParent)
	, m_edit(_T(""))
{

}

CDLGMediaInfo::~CDLGMediaInfo()
{
}

void CDLGMediaInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_edit);
}


BEGIN_MESSAGE_MAP(CDLGMediaInfo, CDialog)
	ON_BN_CLICKED(IDOK, &CDLGMediaInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDLGMediaInfo::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CDLGMediaInfo::OnBnClickedButtonCopy)
END_MESSAGE_MAP()


// CDLGMediaInfo message handlers
BOOL CDLGMediaInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	MediaInfo MI;

	MI.Open(path);
	MI.Option("Complete", "1");
	m_edit = MI.Inform().c_str();
	MI.Close();

	UpdateData(FALSE);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDLGMediaInfo::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

void CDLGMediaInfo::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

#include "video.h"
void CDLGMediaInfo::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here
	AppendBufferToClipboard(m_edit.GetBuffer(0),m_edit.GetLength());
}
