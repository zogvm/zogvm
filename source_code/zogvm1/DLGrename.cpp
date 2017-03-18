// DLGrename.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGrename.h"


// CDLGrename dialog

IMPLEMENT_DYNAMIC(CDLGrename, CDialog)

CDLGrename::CDLGrename(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGrename::IDD, pParent)
	, m_newname(_T(""))
{
	re=-1;

}

CDLGrename::~CDLGrename()
{
}

void CDLGrename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_newname);
		DDV_MaxChars(pDX, m_newname, 256);
}


BEGIN_MESSAGE_MAP(CDLGrename, CDialog)
	ON_BN_CLICKED(IDOK, &CDLGrename::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDLGrename::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDLGrename message handlers
BOOL CDLGrename::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_newname=onlyname;
	this->UpdateData(FALSE);

	return TRUE;
}

void CDLGrename::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	this->UpdateData(TRUE);
	if(m_newname.IsEmpty())
	{
		MessageBox("∫√ø’–È≈∂");
		return ;
	}
	sprintf(newpath,"%s%s",onlypath,m_newname.GetBuffer(0));
	re=rename(oldpath,newpath);

	if(0!=re)
	{
		MessageBox("—≈Û∫µ˚”¥");
		return ;
	}
	OnOK();
}

void CDLGrename::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
