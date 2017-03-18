// DLGAddEd2k.cpp : implementation file
//

#include "stdafx.h"
#include "zogvm.h"
#include "DLGAddEd2k.h"
#include "video.h"


// CDLGAddEd2k dialog

IMPLEMENT_DYNAMIC(CDLGAddEd2k, CDialog)

CDLGAddEd2k::CDLGAddEd2k(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGAddEd2k::IDD, pParent)
	, m_e_txt(_T(""))
	, m_e_db(_T(""))
	, m_c_encode(0)
{

}

CDLGAddEd2k::~CDLGAddEd2k()
{
}

void CDLGAddEd2k::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TXT, m_e_txt);
	DDX_Text(pDX, IDC_EDIT_DB, m_e_db);
	DDX_CBIndex(pDX, IDC_COMBO_ENCODE, m_c_encode);
}


BEGIN_MESSAGE_MAP(CDLGAddEd2k, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TXT, &CDLGAddEd2k::OnBnClickedButtonTxt)

END_MESSAGE_MAP()


// CDLGAddEd2k message handlers

BOOL CDLGAddEd2k::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}




DWORD WINAPI AddEd2kThreadPROC(LPVOID lpParameter)
{
	CDLGAddEd2k *pDlg=(CDLGAddEd2k *)lpParameter;

	pDlg->ed2kList.clear();

	list<struct EXTERN_ED2K_ST>::iterator beglist;
	struct EXTERN_ED2K_ST data={0};
	int i=0;
	long j=0;
	char str[2048]={0};
	wchar_t  wbuff[1024];
	wchar_t  temp[1024];

	FILE *fp=NULL;
	fp=fopen(pDlg->m_e_txt.GetBuffer(0),"r");
	if(!fp)
		goto THREADEXIT;

	else if(ENCODE_UTF8==pDlg->m_c_encode)
	{
		fread(str,1,1,fp);
	}

	if(SQLITE_OK!= pDlg->INDB.OpenDB(pDlg->m_e_db.GetBuffer(0)))
		goto THREADEXIT;


	pDlg->INDB.ExternED2K_CreateTable();
	
	while(fgets(str,sizeof(str),fp))
	{
		if(pDlg->breakflag)
			goto THREADEXIT;

		if(strlen(str)>350)
			continue;

		if(ENCODE_ANSI==pDlg->m_c_encode)
		{
			MultiByteToWideChar(CP_ACP, 0, str, -1, wbuff, 1024);  
			strcpy(data.ed2k,str);
		}
		if(ENCODE_UTF8==pDlg->m_c_encode)
		{
			MultiByteToWideChar(CP_UTF8, 0, str, -1, wbuff, 1024);  
			WideCharToMultiByte(CP_ACP, 0, wbuff, -1, data.ed2k,512, 0, 0); 
		}

		if(wcslen(wbuff)<2)
			continue;

		wchar_t *a=&(wbuff[13]);
		if(NULL==a)
			continue;

		wchar_t *b=wcschr(a,L'|');
		if(NULL==b)
			continue;

		memset(temp,0,sizeof(temp));
		//解析到名字
		wmemcpy(temp,a,wcslen(wbuff)-wcslen(b)-13);
		WideCharToMultiByte(CP_ACP, 0, temp, -1, data.name,256, 0, 0); 

		wchar_t *c=&(b[1]);
		if(NULL==c)
			continue;

		if(!((*c)>=L'0'&& (*c)<=L'9'))
			continue;

		wchar_t *d=wcschr(c,L'|');
		if(NULL==d)
			continue;

		memset(temp,0,sizeof(temp));
		wmemcpy(temp,c,wcslen(b)-wcslen(d)-1);
		
		//解析到文件大小
		swscanf(temp,L"%I64u",&(data.filesize));

		pDlg->ed2kList.push_back(data);
		memset(&data,0,sizeof(struct EXTERN_ED2K_ST));

		if(j==14071)
			j=14071;
		if(i>=200)
		{
			sprintf(str,ConvertString("导入到%d个,本次导入%d个"),j,pDlg->ed2kList.size());
			pDlg->GetDlgItem(IDC_STATIC_INFO)->SetWindowText(str);

			pDlg->INDB.Begin();

			for(beglist=pDlg->ed2kList.begin();beglist!=pDlg->ed2kList.end();beglist++)
			{
				pDlg->INDB.ExternED2K_Add(*beglist);
			}
			pDlg->INDB.Commit();

			pDlg->ed2kList.clear();
			
			i=0;
		}
		else
			i++;

		j++;
	}

	if(pDlg->ed2kList.size()>0)
	{
		pDlg->INDB.Begin();

		for(beglist=pDlg->ed2kList.begin();beglist!=pDlg->ed2kList.end();beglist++)
		{
			pDlg->INDB.ExternED2K_Add(*beglist);
		}
		pDlg->INDB.Commit();
	}

	
THREADEXIT:
	if(NULL!=fp)
		fclose(fp);
	pDlg->GetDlgItem(IDC_STATIC_INFO)->SetWindowText(ConvertString("完成"));
	pDlg->ed2kList.clear();
	pDlg->INDB.CloseDB();
	pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE);

	return 0;
}
void CDLGAddEd2k::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	if(m_e_db.IsEmpty() || m_e_txt.IsEmpty())
	{
		MessageBox(ConvertString("路径不能为空"));
		return ;
	}
	breakflag=false;

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	CreateThread(NULL,0,AddEd2kThreadPROC,this,0,NULL);
	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(ConvertString("开始"));

	//CDialog::OnOK();
}

void CDLGAddEd2k::OnCancel() 
{
	// TODO: Add extra cleanup here
	breakflag=true;
	Sleep(1000);
	ed2kList.clear();
	INDB.CloseDB();
	CDialog::OnCancel();
}

void CDLGAddEd2k::OnBnClickedButtonTxt()
{
		UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	char szFilter[]="Txt Files (*.txt)|*.txt|*.*|*.*||";
	CFileDialog dlg(TRUE,"txt","",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFilter);
	if(dlg.DoModal()==IDOK)
	{
		m_e_txt=dlg.GetPathName();
		UpdateData(FALSE);
	}
}
