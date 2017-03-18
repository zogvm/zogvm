// DLGhdddelete.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGhdddelete.h"
#include "video.h"

extern CSqliteOperate SQLDB;
// CDLGhdddelete dialog

IMPLEMENT_DYNAMIC(CDLGhdddelete, CDialog)

CDLGhdddelete::CDLGhdddelete(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGhdddelete::IDD, pParent)
{
	m_check = FALSE;
}

CDLGhdddelete::~CDLGhdddelete()
{
}

void CDLGhdddelete::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_CHECK, m_check);
}


BEGIN_MESSAGE_MAP(CDLGhdddelete, CDialog)
		ON_BN_CLICKED(IDC_CHECK, OnCheck)
		ON_BN_CLICKED(IDC_BUTTON_DELETE_DOUBLE, &CDLGhdddelete::OnBnClickedButtonDeleteDouble)
		ON_BN_CLICKED(IDC_BUTTON_REBUILD_DOUBLE, &CDLGhdddelete::OnBnClickedButtonRebuildDouble)
END_MESSAGE_MAP()


// CDLGhdddelete message handlers

BOOL CDLGhdddelete::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_list.InsertColumn(0, ConvertString("磁盘序列号") , LVCFMT_LEFT, 90);
	m_list.InsertColumn(1, ConvertString("分区") , LVCFMT_LEFT, 40);
	m_list.InsertColumn(2, ConvertString("标签") , LVCFMT_LEFT, 150);
	m_list.InsertColumn(3, ConvertString("总空间") , LVCFMT_LEFT, 75);
	m_list.InsertColumn(4, ConvertString("剩余空间") , LVCFMT_LEFT, 70);
	m_list.InsertColumn(5, ConvertString("是否接驳") , LVCFMT_LEFT, 60);
	m_list.InsertColumn(6, ConvertString("nid") , LVCFMT_LEFT, 0);
	m_list.InsertColumn(7, ConvertString("格式") , LVCFMT_LEFT, 50);
	//带复选框 LVS_EX_CHECKBOXES
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);



	DisplayerList() ;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDLGhdddelete::DisplayerList() 
{
	hddList.clear();
	SQLDB.Hdd_Read(hddList);
	m_list.DeleteAllItems();

	char str[32];
	int i=0;
	list<struct HDD_ST>::iterator beglist;

	for(beglist=hddList.begin();beglist!=hddList.end();beglist++)
	{
		m_list.InsertItem(i,beglist->serno);

		m_list.SetItemText(i,1,beglist->area);
		m_list.SetItemText(i,2,beglist->mark);

		sprintf(str,"%.3fG",(double)(beglist->TotalBytes/1024/1024)/1024.0);
		m_list.SetItemText(i,3,str);
		sprintf(str,"%.3fG",(double)(beglist->FreeBytes/1024/1024)/1024.0);
		m_list.SetItemText(i,4,str);

		if(beglist->insertflag)
			m_list.SetItemText(i,5,ConvertString("接入<--"));
		else
			m_list.SetItemText(i,5,ConvertString("脱机-->"));

		sprintf(str,"%d",beglist->hdd_nid);
		m_list.SetItemText(i,6,str);

		m_list.SetItemText(i,7,beglist->format);

		i++;
	}
}

void CDLGhdddelete::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	int i;

	char str[32];
	int check;

		SQLDB.Begin();
	for( i=0;i<m_list.GetItemCount();i++)
	{
		m_list.GetItemText(i,6,str,32);
		check=m_list.GetCheck(i);

		if(check)
		{
			SQLDB.File_DeleteforHdd(_atoi64(str));
			SQLDB.Hdd_DeleteforHdd(_atoi64(str));
			SQLDB.Dir_DeleteforHdd(_atoi64(str));
		}
	}
		SQLDB.Commit();

	MessageBox(ConvertString("删除完毕"),NULL);

	hddList.clear();
	SQLDB.Hdd_Read(hddList);
	DisplayerList() ;

}

void CDLGhdddelete::OnCancel() 
{
	// TODO: Add extra cleanup here
	hddList.clear();
	CDialog::OnCancel();
}

void CDLGhdddelete::OnCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list.GetItemCount();i++)
		m_list.SetCheck(i,m_check);
}


void CDLGhdddelete::OnBnClickedButtonDeleteDouble()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	SQLDB.Begin();
	SQLDB.Double_DeleteAll();
	SQLDB.File_CleanDouble2Zero();
	SQLDB.Commit();

	MessageBox(ConvertString("删除完毕"),NULL);
}


DWORD WINAPI DoubleThreadPROC(LPVOID lpParameter)
{
	list<struct HDD_ST> temphddList;
	temphddList.clear();
	SQLDB.Hdd_Read(temphddList);

//	CDLGhdddelete *Pdlg=(CDLGhdddelete *)lpParameter;

	list<struct HDD_ST>::iterator beglist;

	for(beglist=temphddList.begin();beglist!=temphddList.end();beglist++)
	{
		//查看重复文件
		CheckAllDoubleFile(beglist->hdd_nid,"");
	}


	return 0;
}
void CDLGhdddelete::OnBnClickedButtonRebuildDouble()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_REBUILD_DOUBLE)->EnableWindow(FALSE);

	CreateThread(NULL,0,DoubleThreadPROC,this,0,NULL);
	Sleep(1000);
	GetDlgItem(IDC_BUTTON_REBUILD_DOUBLE)->EnableWindow(TRUE);
}
