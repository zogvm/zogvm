// DLGsoap.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGsoap.h"
#include "video.h"
#include "URLencode.h"

extern TCHAR CurrentDir[MAX_PATH*2];
// CDLGsoap dialog

IMPLEMENT_DYNAMIC(CDLGsoap, CDialog)

CDLGsoap::CDLGsoap(CWnd* pParent /*=NULL*/)
: CDialog(CDLGsoap::IDD, pParent)
{
	m_sreach=_T("");
	m_page = 1;
	ListTotal=0;
	ListNow=0;
}

CDLGsoap::~CDLGsoap()
{
	soapDB.CloseDB();
}

void CDLGsoap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SREACH, m_sreach);
	DDV_MaxChars(pDX, m_sreach, 100);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDIT_PAGE, m_page);
	DDX_Control(pDX, IDC_BUTTON_FIRST, m_first_button);
	DDX_Control(pDX, IDC_BUTTON_PREVIOUS, m_pre_button);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_next_button);
	DDX_Control(pDX, IDC_BUTTON_LAST, m_last_button);
	DDX_Control(pDX, IDC_BUTTON_JUMP, m_jump_button);
	DDX_Check(pDX, IDC_CHECK, m_check);
}


BEGIN_MESSAGE_MAP(CDLGsoap, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &CDLGsoap::OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_ED2K, &CDLGsoap::OnBnClickedButtonEd2k)
	ON_BN_CLICKED(IDC_BUTTON_FIRST, OnButtonFirst)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, OnButtonPrevious)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_LAST, OnButtonLast)
	ON_BN_CLICKED(IDC_BUTTON_JUMP, OnButtonJump)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)

	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CDLGsoap::OnInitDialog()
{
	CDialog::OnInitDialog();

	char tempstr[520];
	sprintf(tempstr,"%s\\externdb\\zogvmsoap.db",CurrentDir);
	FILE* fp=fopen(tempstr,"rb");
	if(fp)
	{
		fclose(fp);
		soapDB.OpenDB(tempstr);

	}
	else
	{
		MessageBox(ConvertString("数据库未找到"),"error");
		return FALSE;
	}

	m_list.InsertColumn(0, ConvertString("序列号") , LVCFMT_LEFT, 70);
	m_list.InsertColumn(1, ConvertString("片名") , LVCFMT_LEFT, 600);
	m_list.InsertColumn(2, ConvertString("容量") , LVCFMT_LEFT, 70);
	m_list.InsertColumn(3, ConvertString("ed2k") , LVCFMT_LEFT, 100);
	m_list.InsertColumn(4, ConvertString("aich") , LVCFMT_LEFT, 0);
	m_list.InsertColumn(5, ConvertString("btih") , LVCFMT_LEFT, 0);

	//带复选框 LVS_EX_CHECKBOXES
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);


	//初始即最大化 //放最后
	//ShowWindow(SW_MAXIMIZE);   

	OnBnClickedButtonFind();

	return TRUE;
}
void CDLGsoap::OnOK() 
{
	// TODO: Add extra validation here

	//	CDialog::OnOK();
}
void CDLGsoap::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect		m_clientRect;		//程序界面区域位置
	GetClientRect(&m_clientRect);

	int height=70;

	CRect list_Rect;
	list_Rect.top = 	m_clientRect.top+height;
	list_Rect.bottom = m_clientRect.bottom-30;
	list_Rect.left = m_clientRect.left ;
	list_Rect.right = m_clientRect.right;
	//必须 样式=重叠，边框=调整大小
	m_list.MoveWindow(list_Rect);


	int buttonbmpw=24,buttonbmph=18,buttongap=10;

	CRect b_Rect;

	b_Rect.top = list_Rect.bottom+5 ;
	b_Rect.bottom =	b_Rect.top+buttonbmph;

	b_Rect.left = list_Rect.left+10;
	b_Rect.right = b_Rect.left+300;
	GetDlgItem(IDC_STATIC_INFO)->MoveWindow(b_Rect);

	printf_Rect=b_Rect;

	b_Rect.left = b_Rect.right+buttongap;
	b_Rect.right = b_Rect.left+buttonbmpw;
	m_first_button.MoveWindow(b_Rect);

	b_Rect.left = b_Rect.right+buttongap;
	b_Rect.right = b_Rect.left+buttonbmpw;
	m_pre_button.MoveWindow(b_Rect);

	b_Rect.left = b_Rect.right+buttongap;
	b_Rect.right = b_Rect.left+buttonbmpw;
	m_next_button.MoveWindow(b_Rect);

	b_Rect.left = b_Rect.right+buttongap;
	b_Rect.right = b_Rect.left+buttonbmpw;
	m_last_button.MoveWindow(b_Rect);
	//////////////

	b_Rect.top = list_Rect.bottom+8 ;
	b_Rect.left = b_Rect.right+buttongap*3;
	b_Rect.right = b_Rect.left+20;
	GetDlgItem(IDC_STATIC_DI)->MoveWindow(b_Rect);

	b_Rect.top = list_Rect.bottom+5 ;
	b_Rect.left = b_Rect.right;
	b_Rect.right = b_Rect.left+60;
	GetDlgItem(IDC_EDIT_PAGE)->MoveWindow(b_Rect);

	b_Rect.top = list_Rect.bottom+2 ;
	b_Rect.bottom =	b_Rect.top+24;
	b_Rect.left = b_Rect.right;
	b_Rect.right = b_Rect.left+20;
	m_jump_button.MoveWindow(b_Rect);

	b_Rect.top = list_Rect.bottom+8 ;
	b_Rect.left = b_Rect.right;
	b_Rect.right = b_Rect.left+20;
	GetDlgItem(IDC_STATIC_YE)->MoveWindow(b_Rect);

	Invalidate();
}

// CDLGsoap message handlers

void CDLGsoap::OnBnClickedButtonFind()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	ListTotal=0;

	if(m_sreach.IsEmpty())
		ListTotal=soapDB.SOAP_Number();
	else
		ListTotal=soapDB.SOAP_NumberForSearch(m_sreach.GetBuffer(0));

	ListNow=0;

	DisplayerList();
}
void CDLGsoap::OnBnClickedButtonEd2k()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int i;
	CString str="";
	char tempstr[32]={0};
	long long nid;
	char url[1024]={0};

	if(soapList.size() <=0)
		return ;

	list<struct FILE_SOAP_READ_ST>::iterator beglist;

	// ClearClipboradBuffer();

	for( i=0;i<m_list.GetItemCount();i++)
	{
		if(m_list.GetCheck(i))
		{
			m_list.GetItemText(i,0,tempstr,32);
			sscanf(tempstr,"%I64u",&nid);

			for(beglist=soapList.begin();beglist!=soapList.end();beglist++)
			{
				if(beglist->file_nid == nid)
				{
					if(strlen(beglist->ed2k)>0)
					{
						EncodeURI(beglist->name,url,1024);

						sprintf(beglist->ed2kurl,"ed2k://|file|%s|%I64u|%s|h=%s",url,beglist->filesize,beglist->ed2k,beglist->aich);
						str+=beglist->ed2kurl;
						str+="\r\n";
					}
					break;
				}
			}
		}
	}
	if(str.IsEmpty())
		return ;
	AppendBufferToClipboard(str.GetBuffer(0),str.GetLength());
}

void CDLGsoap::DisplaySize(long long size,char* str)
{
	long long temp=size/1024;

	if(temp==0)
	{
		sprintf(str,"%I64u",size);
		return ;
	}
	temp/=1024;
	if(temp==0)
	{
		sprintf(str,"%I64u K",size/1024);
		return ;
	}
	temp/=1024;
	if(temp==0)
	{
		sprintf(str,"%I64u M",size/(1024*1024));
		return ;
	}

	sprintf(str,"%.02lf G",(double)(size/1024/1024)/1024.0);
}

void CDLGsoap::DisplayerList()
{
	m_list.DeleteAllItems();
	soapList.clear();

	if(0==ListTotal)
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("共0条 1/1页");
		InvalidateRect(printf_Rect, TRUE);
		return ;
	}

	unsigned long int si,ei;

	si=ListNow+1;
	if((ListNow+PAGE_MAX_NUM) < ListTotal)
		ei=ListNow+PAGE_MAX_NUM ;
	else
		ei=ListTotal;


	if(m_sreach.IsEmpty())
		soapDB.SOAP_Read(soapList,si-1,ei-(si-1));
	else
		soapDB.SOAP_ReadForSearch(m_sreach.GetBuffer(0),soapList,si-1,ei-(si-1));

	if(soapList.size()<=0)
		return ;

	list<struct FILE_SOAP_READ_ST>::iterator beglist;
	char str[128];
	int i=0;

	for(beglist=soapList.begin();beglist!=soapList.end();beglist++)
	{
		sprintf(str,"%I64u",beglist->file_nid);
		m_list.InsertItem( i,str);
		m_list.SetItemText(i,1,beglist->name);
		DisplaySize(beglist->filesize,str);

		m_list.SetItemText(i,2,str);
		m_list.SetItemText(i,3,beglist->ed2k);
		m_list.SetItemText(i,4,beglist->aich);
		m_list.SetItemText(i,5,beglist->btih);
		i++;
	}

	if(ListTotal%PAGE_MAX_NUM)
	{
		sprintf(str,ConvertString("共%d条 %d/%d页 每页%d条"),
			ListTotal,
			ListNow/PAGE_MAX_NUM+1,ListTotal/PAGE_MAX_NUM+1,PAGE_MAX_NUM);
	}
	else
	{
		sprintf(str,ConvertString("共%d条 %d/%d页 每页%d条"),
			ListTotal,
			ListNow/PAGE_MAX_NUM+1,ListTotal/PAGE_MAX_NUM,PAGE_MAX_NUM);
	}
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(str);
	InvalidateRect(printf_Rect, TRUE);


	UpdateData(FALSE);
}


void CDLGsoap::OnButtonFirst() 
{
	// TODO: Add your control notification handler code here
	if(0==ListTotal)
		return ;

	ListNow=0;
	DisplayerList();
}

void CDLGsoap::OnButtonPrevious() 
{
	// TODO: Add your control notification handler code here
	if(0==ListTotal)
		return ;

	if((ListNow-PAGE_MAX_NUM) >= 0)
		ListNow-=PAGE_MAX_NUM;

	DisplayerList();
}

void CDLGsoap::OnButtonNext() 
{
	// TODO: Add your control notification handler code here
	if(0==ListTotal)
		return ;

	if((ListNow+PAGE_MAX_NUM) < ListTotal)
		ListNow+=PAGE_MAX_NUM;

	DisplayerList();
}

void CDLGsoap::OnButtonLast() 
{
	// TODO: Add your control notification handler code here
	if(0==ListTotal)
		return ;

	while((ListNow+PAGE_MAX_NUM) < ListTotal)
		ListNow+=PAGE_MAX_NUM;

	DisplayerList();
}

void CDLGsoap::OnButtonJump() 
{
	// TODO: Add your control notification handler code here
	if(0==ListTotal)
		return ;

	UpdateData(TRUE);

	if((m_page>=1)
		&& (m_page <= (ListTotal/PAGE_MAX_NUM+1))
		)
	{
		ListNow=0;
		while(((ListNow+PAGE_MAX_NUM) < ListTotal)
			&& ((ListNow+PAGE_MAX_NUM) < (PAGE_MAX_NUM*m_page))
			)
			ListNow+=PAGE_MAX_NUM;

		DisplayerList();
	}
	else
		MessageBox(ConvertString("不在页面范围"));

}


void CDLGsoap::OnCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list.GetItemCount();i++)
		m_list.SetCheck(i,m_check);
}


BOOL CDLGsoap::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{

		case VK_UP:
		case VK_HOME:
			OnButtonFirst();
			break;
		case VK_LEFT:
		case VK_PRIOR:
			OnButtonPrevious() ;
			break;
		case VK_RIGHT:
		case VK_NEXT:
			OnButtonNext() ;

			break;

		case VK_DOWN:
		case VK_END:
			OnButtonLast() ;
			break;
		default:break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
