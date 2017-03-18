// DLGset.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGset.h"


extern CSqliteOperate SQLDB;
extern struct SYSTEM_SET_ST systemset;

// DLGset dialog

IMPLEMENT_DYNAMIC(DLGset, CDialog)

DLGset::DLGset(CWnd* pParent /*=NULL*/)
	: CDialog(DLGset::IDD, pParent)
	
	, m_c_mediainfo(FALSE)
{
	m_c_net_proxy=systemset.checkProxy;
	m_e_net_proxy=systemset.Proxy;
	if(systemset.subsrc ==SUBSRC_SV)
		m_c_sub_sv=TRUE;
	else
		m_c_sub_sv=FALSE;

	m_c_svsub_onlyone=systemset.checkOnlyOne;
	m_c_svsub_onlychn=systemset.checkOnlyChn;
	m_c_mediainfo=systemset.c_mediainfo;
}

DLGset::~DLGset()
{
}

void DLGset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_NET_PROXY, m_c_net_proxy);
	DDX_Text(pDX, IDC_EDIT_NET_PROXY, m_e_net_proxy);
	DDX_Check(pDX, IDC_CHECK_SUB_SV, m_c_sub_sv);
	DDX_Check(pDX, IDC_CHECK_SVSUB_ONLEYONE, m_c_svsub_onlyone);
	DDX_Check(pDX, IDC_CHECK_SVSUB_ONLYCHN, m_c_svsub_onlychn);
	DDX_Check(pDX, IDC_CHECK_MEDIAINFO, m_c_mediainfo);
}


BEGIN_MESSAGE_MAP(DLGset, CDialog)
END_MESSAGE_MAP()


BOOL DLGset::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_INFO)->SetWindowText("免责条款:\n\
本软件所有资源不由本软件提供，均由网友自行分享与交流。\n\
本软件所列出的各资源文件并没有保存在作者的服务器上。\n\
本软件仅负责连接，我们对被传播文件的内容一无所知，作者无法对用户的任何法律责任行为而负责。\n\
郑重声明:\n\
本软件提供的所有资源均是在网上搜集而来。仅做数据库编程学习使用。\n\
任何涉及商业盈利目的均不得使用，否则产生的一切后果将由您自己承担！\n\
作者将不对本软件的任何内容负任何法律责任！\n\
该下载内容仅做宽带测试使用，请在下载后24小时内删除。请支持并购买正版！");

	return TRUE;  // return TRUE  unless you set the focus to a control
}
// DLGset message handlers

void DLGset::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	systemset.checkProxy=m_c_net_proxy;
	strcpy(systemset.Proxy,m_e_net_proxy.GetBuffer(0));

	if(	m_c_sub_sv)
		systemset.subsrc =SUBSRC_SV;

	systemset.checkOnlyOne=m_c_svsub_onlyone;
	systemset.checkOnlyChn=	m_c_svsub_onlychn;

	systemset.c_mediainfo=m_c_mediainfo;

	SQLDB.SystemSet_Modify(systemset);

	CDialog::OnOK();
}

void DLGset::OnCancel() 
{
	
	CDialog::OnCancel();
}
