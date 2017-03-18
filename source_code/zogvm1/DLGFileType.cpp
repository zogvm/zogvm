// DLGFileType.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGFileType.h"
#include "video.h"

#include "SqliteOperate.h"
extern CSqliteOperate SQLDB;
extern struct SYSTEM_SET_ST systemset;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGFileType dialog


CDLGFileType::CDLGFileType(CWnd* pParent /*=NULL*/)
: CDialog(CDLGFileType::IDD, pParent)
, m_e_new(_T(""))
, m_c_new(0)
, m_c_noscan_video_min(FALSE)
, m_e_noscan_video_min(0)
, m_c_noscan_video_max(FALSE)
, m_e_noscan_video_max(0)
, m_c_noscan_audio_min(FALSE)
, m_e_noscan_audio_min(0)
, m_c_noscan_audio_max(FALSE)
, m_e_noscan_audio_max(0)
{
	//{{AFX_DATA_INIT(CDLGFileType)
	m_c_music = FALSE;
	m_c_sub = FALSE;
	m_c_video = FALSE;
	m_c_other = FALSE;
	//}}AFX_DATA_INIT
}


void CDLGFileType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGFileType)
	DDX_Control(pDX, IDC_LIST_OTHER, m_list_other);
	DDX_Control(pDX, IDC_LIST_VIDEO, m_list_video);
	DDX_Control(pDX, IDC_LIST_SUB, m_list_sub);
	DDX_Control(pDX, IDC_LIST_MUSIC, m_list_music);
	DDX_Check(pDX, IDC_CHECK_MUSIC, m_c_music);
	DDX_Check(pDX, IDC_CHECK_SUB, m_c_sub);
	DDX_Check(pDX, IDC_CHECK_VIDEO, m_c_video);
	DDX_Check(pDX, IDC_CHECK_OTHER, m_c_other);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_NEW, m_e_new);
	DDX_CBIndex(pDX, IDC_COMBO_NEW, m_c_new);
	DDX_Check(pDX, IDC_CHECK_NOSCAN_VIDEO_MIN, m_c_noscan_video_min);
	DDX_Text(pDX, IDC_EDIT_NOSCAN_VIDEO_MIN, m_e_noscan_video_min);
	DDX_Check(pDX, IDC_CHECK_NOSCAN_VIDEO_MAX, m_c_noscan_video_max);
	DDX_Text(pDX, IDC_EDIT_NOSCAN_VIDEO_MAX, m_e_noscan_video_max);
	DDX_Check(pDX, IDC_CHECK_NOSCAN_AUDIO_MIN, m_c_noscan_audio_min);
	DDX_Text(pDX, IDC_EDIT_NOSCAN_AUDIO_MIN, m_e_noscan_audio_min);
	DDX_Check(pDX, IDC_CHECK_NOSCAN_AUDIO_MAX, m_c_noscan_audio_max);
	DDX_Text(pDX, IDC_EDIT_NOSCAN_AUDIO_MAX, m_e_noscan_audio_max);
}


BEGIN_MESSAGE_MAP(CDLGFileType, CDialog)
//{{AFX_MSG_MAP(CDLGFileType)
ON_BN_CLICKED(IDC_CHECK_VIDEO, OnCheckVideo)
ON_BN_CLICKED(IDC_CHECK_SUB, OnCheckSub)
ON_BN_CLICKED(IDC_CHECK_MUSIC, OnCheckMusic)
ON_BN_CLICKED(IDC_CHECK_OTHER, OnCheckOther)
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_BUTTON_NEW, &CDLGFileType::OnBnClickedButtonNew)
ON_BN_CLICKED(IDC_BUTTON_REBUILD, &CDLGFileType::OnBnClickedButtonRebuild)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGFileType message handlers
BOOL CDLGFileType::OnInitDialog()
{
	CDialog::OnInitDialog();
	typeList.clear();
	SQLDB.Type_Read(typeList); 
	
	//初始化列表
	InitVideoList();
	DisplayVideoList();

	InitMusicList();
	DisplayMusicList();

	InitSubList();
	DisplaySubList();

	InitOtherList();
	DisplayOtherList();

	m_c_noscan_video_min=systemset.c_noscan_video_min;
	 m_e_noscan_video_min=systemset.e_noscan_video_min;
	 m_c_noscan_video_max=systemset.c_noscan_video_max;
	m_e_noscan_video_max=systemset.e_noscan_video_max;
	m_c_noscan_audio_min=systemset.c_noscan_audio_min;
	m_e_noscan_audio_min=systemset.e_noscan_audio_min;
	m_c_noscan_audio_max=systemset.c_noscan_audio_max;
	m_e_noscan_audio_max=systemset.e_noscan_audio_max;
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDLGFileType::InitVideoList(void)
{
	m_list_video.InsertColumn(0, ConvertString("后缀") , LVCFMT_LEFT, 100);
	m_list_video.InsertColumn(1, ConvertString("nid") , LVCFMT_LEFT, 0);
	//带复选框 LVS_EX_CHECKBOXES
	m_list_video.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	
}
void CDLGFileType::DisplayVideoList(void)
{
	m_list_video.DeleteAllItems();
	char str[32];
	int i=0;
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->maintype == MAINTYPE_VIDEO)
		{
			m_list_video.InsertItem(i,beglist->type);

			sprintf(str,"%d",beglist->type_nid);
			m_list_video.SetItemText(i,1,str);

			m_list_video.SetCheck(i,beglist->enable);
			i++;
		}
	}
}

void CDLGFileType::InitMusicList(void)
{
	m_list_music.InsertColumn(0, ConvertString("后缀") , LVCFMT_LEFT, 100);
	m_list_music.InsertColumn(1, ConvertString("nid") , LVCFMT_LEFT, 0);
	
	//带复选框 LVS_EX_CHECKBOXES
	m_list_music.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	

}
void CDLGFileType::DisplayMusicList(void)
{
	m_list_music.DeleteAllItems();

	char str[32];
	int i=0;
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->maintype == MAINTYPE_MUSIC)
		{
			m_list_music.InsertItem(i,beglist->type);

			sprintf(str,"%d",beglist->type_nid);
			m_list_music.SetItemText(i,1,str);
			m_list_music.SetCheck(i,beglist->enable);
			i++;
		}
	}

}


void CDLGFileType::InitSubList(void)
{
	m_list_sub.InsertColumn(0, ConvertString("后缀") , LVCFMT_LEFT, 100);
	m_list_sub.InsertColumn(1, ConvertString("nid") , LVCFMT_LEFT, 0);
	//带复选框 LVS_EX_CHECKBOXES
	m_list_sub.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

}
void CDLGFileType::DisplaySubList(void)
{
		m_list_sub.DeleteAllItems();
	char str[32];
	int i=0;
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->maintype == MAINTYPE_SUB)
		{
			m_list_sub.InsertItem(i,beglist->type);
			sprintf(str,"%d",beglist->type_nid);
			m_list_sub.SetItemText(i,1,str);
			m_list_sub.SetCheck(i,beglist->enable);
			i++;
		}
	}
}

void CDLGFileType::InitOtherList(void)
{
	m_list_other.InsertColumn(0, ConvertString("后缀") , LVCFMT_LEFT, 100);
	m_list_other.InsertColumn(1, ConvertString("nid") , LVCFMT_LEFT, 0);
	
	//带复选框 LVS_EX_CHECKBOXES
	m_list_other.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

}
void CDLGFileType::DisplayOtherList(void)
{
	m_list_other.DeleteAllItems();
	char str[32];
	int i=0;
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->maintype == MAINTYPE_OTHER)
		{
			m_list_other.InsertItem(i,beglist->type);
			sprintf(str,"%d",beglist->type_nid);
			m_list_other.SetItemText(i,1,str);
			m_list_other.SetCheck(i,beglist->enable);
			i++;
		}
	}
}

void CDLGFileType::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	int i;
	if(m_e_noscan_video_min<0 || m_e_noscan_video_min >1000000 ||
		m_e_noscan_video_max<0 || m_e_noscan_video_max >1000000 ||
		m_e_noscan_audio_min<0 || m_e_noscan_audio_min >1000000 ||
		m_e_noscan_audio_max<0 || m_e_noscan_audio_max >1000000 
		)
	{
		MessageBox("值在0-1000000之间");
		return ;
	}

	char str[32];
	int check;

	SQLDB.Begin();
	for( i=0;i<m_list_video.GetItemCount();i++)
	{
		m_list_video.GetItemText(i,1,str,32);
		check=m_list_video.GetCheck(i);
		SQLDB.Type_ModifyEnable(str,check);
	}
	for( i=0;i<m_list_music.GetItemCount();i++)
	{
		m_list_music.GetItemText(i,1,str,32);
		check=m_list_music.GetCheck(i);
		SQLDB.Type_ModifyEnable(str,check);
	}
	for( i=0;i<m_list_sub.GetItemCount();i++)
	{
		m_list_sub.GetItemText(i,1,str,32);
		check=m_list_sub.GetCheck(i);
		SQLDB.Type_ModifyEnable(str,check);
	}
	for( i=0;i<m_list_other.GetItemCount();i++)
	{
		m_list_other.GetItemText(i,1,str,32);
		check=m_list_other.GetCheck(i);
		SQLDB.Type_ModifyEnable(str,check);
	}
	SQLDB.Commit();

	typeList.clear();

	systemset.c_noscan_video_min=m_c_noscan_video_min;
	systemset.e_noscan_video_min =m_e_noscan_video_min;
	systemset.c_noscan_video_max= m_c_noscan_video_max;
	systemset.e_noscan_video_max =m_e_noscan_video_max;
	systemset.c_noscan_audio_min= m_c_noscan_audio_min;
	systemset.e_noscan_audio_min =m_e_noscan_audio_min;
	systemset.c_noscan_audio_max= m_c_noscan_audio_max;
	systemset.e_noscan_audio_max =m_e_noscan_audio_max;

	SQLDB.SystemSet_Modify(systemset);

	CDialog::OnOK();
}

void CDLGFileType::OnCancel() 
{
	// TODO: Add extra cleanup here
	typeList.clear();
	CDialog::OnCancel();
}

void CDLGFileType::OnCheckVideo() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list_video.GetItemCount();i++)
		m_list_video.SetCheck(i,m_c_video);
}

void CDLGFileType::OnCheckSub() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list_sub.GetItemCount();i++)
		m_list_sub.SetCheck(i,m_c_sub);
}

void CDLGFileType::OnCheckMusic() 
{	
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list_music.GetItemCount();i++)
		m_list_music.SetCheck(i,m_c_music);
}

void CDLGFileType::OnCheckOther() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_list_other.GetItemCount();i++)
		m_list_other.SetCheck(i,m_c_other);
}

void CDLGFileType::OnBnClickedButtonNew()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_e_new.IsEmpty())
	{
		MessageBox(ConvertString("文件后缀不能为空"));
		return ;
	}
	SQLDB.Type_Add_Lock(m_c_new,m_e_new.GetBuffer(0));

	typeList.clear();
	SQLDB.Type_Read(typeList); 

	//初始化列表
	DisplayVideoList();
	DisplayMusicList();
	DisplaySubList();
	DisplayOtherList();

}

void CDLGFileType::OnBnClickedButtonRebuild()
{
	// TODO: Add your control notification handler code here

	SQLDB.Type_DeleteAll();
	SQLDB.Type_AddAll();

	typeList.clear();
	SQLDB.Type_Read(typeList); 

	//初始化列表
	DisplayVideoList();
	DisplayMusicList();
	DisplaySubList();
	DisplayOtherList();
}
