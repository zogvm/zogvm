// DLGxunleisub.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGxunleisub.h"
#include "video.h"

// CDLGxunleisub dialog

IMPLEMENT_DYNAMIC(CDLGxunleisub, CDialog)

CDLGxunleisub::CDLGxunleisub(CWnd* pParent /*=NULL*/)
: CDialog(CDLGxunleisub::IDD, pParent)
, m_edit(_T(""))
{
Choose=-1;
}

CDLGxunleisub::~CDLGxunleisub()
{
}

void CDLGxunleisub::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_edit);
	DDV_MaxChars(pDX, m_edit, 256);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CDLGxunleisub, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDLGxunleisub::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDLGxunleisub::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FIND, &CDLGxunleisub::OnBnClickedFind)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CDLGxunleisub::OnNMClickList)
END_MESSAGE_MAP()


// CDLGxunleisub message handlers
BOOL CDLGxunleisub::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.InsertColumn(0, "序号" , LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, "片名" , LVCFMT_LEFT, 470);
	m_list.InsertColumn(2, "后缀" , LVCFMT_LEFT, 50);
	m_list.InsertColumn(3, "语言" , LVCFMT_LEFT, 90);
	m_list.InsertColumn(4, "url" , LVCFMT_LEFT, 0);

	//带复选框 LVS_EX_CHECKBOXES
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	OnBnClickedFind();

	return TRUE;
}

void CDLGxunleisub::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDLGxunleisub::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
	char cid[8];
	char  ext[8]; //类型
	char  language[32]; //语言
	char url[520];
	m_list.GetItemText(Choose,4,url,520);
	m_list.GetItemText(Choose,2,ext,8);
	m_list.GetItemText(Choose,3,language,32);
	m_list.GetItemText(Choose,0,cid,8);

	char subpath[520];
	CString pathOutExt=path;
	pathOutExt=pathOutExt.Left(pathOutExt.ReverseFind('.'));

	sprintf(subpath,"%s.%s%s.%s",pathOutExt.GetBuffer(0),language,cid,ext);

	bool re=SVsubDownload_Get(url,subpath);

	if(re)
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("下载成功");
	}
	else
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("下载失败,片源文件夹可能不存在哦");
	}
}

void CDLGxunleisub::OnBnClickedFind()
{
	// TODO: Add your control notification handler code here
	this->UpdateData(TRUE);

	m_list.DeleteAllItems();

	list<struct XUNLEI_SUB_ST> subList;
	int re=GetXunleiSub(m_edit.GetBuffer(0),subList);
	if(re<0)
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("网络不通，在系统设置里 是否需要代理？");
		return ;
	}
	else if(0==re)
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("找不到数据");
		return ;
	}

	list<struct XUNLEI_SUB_ST>::iterator beglist;

	int i=0;
	for(beglist=subList.begin();beglist!=subList.end();beglist++)
	{
		m_list.InsertItem( i,&(beglist->cid[strlen(beglist->cid)-3]));
		m_list.SetItemText(i,1,beglist->name);
		m_list.SetItemText(i,2,beglist->ext);
		m_list.SetItemText(i,3,beglist->language);
		m_list.SetItemText(i,4,beglist->url);
		i++;

	}

	subList.clear();

	Choose=-1;
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}

void CDLGxunleisub::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* p=	(NM_LISTVIEW *)pNMHDR;
	//选中的赋值
	Choose= p->iItem;
	//没中
	if(-1 == Choose)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		return ;
	}
	else
		GetDlgItem(IDOK)->EnableWindow(TRUE);
}
