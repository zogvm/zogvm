#include "stdafx.h"
#include "usn.h"

extern CSqliteOperate SQLDB;
extern TCHAR CurrentDir[MAX_PATH*2];

extern struct SYSTEM_SET_ST systemset;

//https://msdn.microsoft.com/en-us/library/aa365736%28v=VS.85%29.aspx
//#include <winioctl.h>
bool IsNtfs(char *str)
{
	if (0==strcmp(str, "NTFS" ))
		return true;
	else
		return false;
}

bool GetVolumesHandle(char c,HANDLE *hVol)
{
	char path[32];
	// 传入的文件名必须为\\.\C:的形式  
	sprintf(path,"\\\\.\\%c:",c);

	(*hVol)=NULL;
	// 调用该函数需要管理员权限  
	(*hVol) = CreateFileA(path, 
		GENERIC_READ | GENERIC_WRITE, // 可以为0  
		//0,
		FILE_SHARE_READ | FILE_SHARE_WRITE, // 必须包含有FILE_SHARE_WRITE  
		NULL, // 这里不需要  
		OPEN_EXISTING, // 必须包含OPEN_EXISTING, CREATE_ALWAYS可能会导致错误  
		FILE_ATTRIBUTE_READONLY, // FILE_ATTRIBUTE_NORMAL可能会导致错误  
		//FILE_FLAG_BACKUP_SEMANTICS,
		NULL); // 这里不需要  

	if(INVALID_HANDLE_VALUE!=(*hVol)) 
		return true;
	else
		return false;
}

bool  OpenUSNJournal(HANDLE hVol) 
{
	DWORD br;  

	CREATE_USN_JOURNAL_DATA cujd;  
	cujd.MaximumSize = 0; // 0表示使用默认值  
	cujd.AllocationDelta = 0; // 0表示使用默认值  
	BOOL status = DeviceIoControl(hVol,  
		FSCTL_CREATE_USN_JOURNAL,  
		&cujd,  
		sizeof(CREATE_USN_JOURNAL_DATA),  
		NULL,  
		0,  
		&br,  
		NULL);  

	if(0!=status)
		return true;
	else
		return false;
}

bool QueryUSNJournal(HANDLE hVol,USN_JOURNAL_DATA *UsnInfo)
{
	DWORD br;  
	BOOL status = DeviceIoControl(hVol,  
		FSCTL_QUERY_USN_JOURNAL,  
		NULL,  
		0,  
		UsnInfo,  
		sizeof(USN_JOURNAL_DATA),  
		&br,  
		NULL);  

	if(0!=status)
		return true;
	else
		return false;
}


bool QueryUSNJournalVolume(HANDLE hVol, NTFS_VOLUME_DATA_BUFFER *ntfsVolData)
{
	DWORD br;             
	BOOL status =DeviceIoControl(hVol,
		FSCTL_GET_NTFS_VOLUME_DATA, 
		NULL, 0, 
		ntfsVolData, sizeof(NTFS_VOLUME_DATA_BUFFER), 
		&br, NULL);

	if(0!=status)
		return true;
	else
		return false;
}



//数据插入
void USNDirFileTreeAdd( DWORDLONG FileReferenceNumber,
					   DWORDLONG ParentFileReferenceNumber,
					   DWORD FileAttributes,
					   char *name,
					   unsigned long long *filetreetotal,
					   unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree)
{
	unsigned long long i;
	if(FileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{	
		i=(*dirtreetotal);

		if(0== i)
		{
			(*dirtreemax)=2000;
			(*dirtree)=(struct USNDirTree *)calloc((*dirtreemax),sizeof(struct USNDirTree));
		}
		if(i>=(*dirtreemax))
		{
			(*dirtreemax)+=500;
			(*dirtree)=(struct USNDirTree *)realloc((*dirtree),(*dirtreemax)*sizeof(struct USNDirTree));
		}
		(*dirtree)[i].ParentFileReferenceNumber=ParentFileReferenceNumber;
		(*dirtree)[i].FileReferenceNumber=FileReferenceNumber;
		strcpy((*dirtree)[i].path,name);
		(*dirtree)[i].flag=false;
		(*dirtreetotal)++;

	}
	else
	{
		i=(*filetreetotal);
		if(0==i)
		{
			(*filetreemax)=2000;
			(*filetree)=(struct USNFileTree *)calloc(	(*filetreemax),sizeof(struct USNFileTree));
		}
		if(i>=	(*filetreemax))
		{
			(*filetreemax)+=500;
			(*filetree)=(struct USNFileTree *)realloc((*filetree),	(*filetreemax)*sizeof(struct USNFileTree));
		}
		(*filetree)[i].ParentFileReferenceNumber=ParentFileReferenceNumber;
		(*filetree)[i].FileReferenceNumber=FileReferenceNumber;
		strcpy((*filetree)[i].name,name);

		(*filetreetotal)++;
	}
}
//递归建立绝对路径
void USNBuildDirTree(DWORDLONG ParentFileReferenceNumber,
					 char *name,
					 unsigned long long dirtreetotal,
struct USNDirTree *dirtree)
{
	unsigned long long i;
	char tempstr[512];
	for( i=0;i<dirtreetotal;i++)
	{
		if(false==dirtree[i].flag)
		{
			if(dirtree[i].ParentFileReferenceNumber==ParentFileReferenceNumber)
			{
				sprintf(tempstr,"%s\\%s",name,dirtree[i].path);
				strcpy(dirtree[i].path,tempstr);
				dirtree[i].flag=true;
				USNBuildDirTree(dirtree[i].FileReferenceNumber,dirtree[i].path,dirtreetotal,dirtree);
			}
		}
	}
}

//遍历DIR建立绝对路径
void USNBuildFileTree(DWORDLONG RootFileReferenceNumber,
					  char *Rootname,
					  unsigned long long filetreetotal,
struct USNFileTree *filetree,
	unsigned long long dirtreetotal,
struct USNDirTree *dirtree)
{
	unsigned long long i,j;
	for( i=0;i<filetreetotal;i++)
	{
		if(filetree[i].ParentFileReferenceNumber==RootFileReferenceNumber)
		{
			sprintf(filetree[i].path,"%s\\%s",Rootname,filetree[i].name);
		}
		else
		{
			for( j=0;j<dirtreetotal;j++)
			{
				if(filetree[i].ParentFileReferenceNumber==dirtree[j].FileReferenceNumber)
				{
					sprintf(filetree[i].path,"%s\\%s",dirtree[j].path,filetree[i].name);
					break;
				}
			}
		}
	}
}
#if 0
//遍历获取文件夹名 文件名 本节点 父节点编号
void GetUSNJournal(HANDLE hVol,USN_JOURNAL_DATA UsnInfo,NTFS_VOLUME_DATA_BUFFER ntfsVolData)
{
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;
	med.HighUsn = UsnInfo.NextUsn;

	// Process MFT in 64k chunks
	BYTE pData[sizeof(DWORDLONG) + 0x10000];
	DWORDLONG fnLast = 0;
	DWORD cb;
	char fileName[MAX_PATH] = {0};  
	while (DeviceIoControl(hVol, FSCTL_ENUM_USN_DATA, &med, sizeof(med),
		pData, sizeof(pData), &cb, NULL) != FALSE)
	{

		PUSN_RECORD pRecord = (PUSN_RECORD) &pData[sizeof(USN)];
		while ((PBYTE) pRecord < (pData + cb))
		{

			if (0 != (pRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString sz((LPCWSTR)
					((PBYTE) pRecord + pRecord->FileNameOffset),
					pRecord->FileNameLength / sizeof(WCHAR));
				//Add(pRecord->FileReferenceNumber, sz,
				//	pRecord->ParentFileReferenceNumber);
			}

			pRecord = (PUSN_RECORD) ((PBYTE) pRecord + pRecord->RecordLength);
		}
		med.StartFileReferenceNumber = * (DWORDLONG *) pData;
	}
}
#endif

//遍历获取文件夹名 文件名 本节点 父节点编号
void GetUSNJournalTest(HANDLE hVol,USN_JOURNAL_DATA UsnInfo,NTFS_VOLUME_DATA_BUFFER ntfsVolData,
					   unsigned long long *filetreetotal,
					   unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree)

{
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;
	med.HighUsn = UsnInfo.NextUsn;

	// Process MFT in 64k chunks
	BYTE pData[sizeof(DWORDLONG) + 0x10000];
	DWORDLONG fnLast = 0;
	DWORD cb;
	char fileName[MAX_PATH*2] = {0};  
	while (DeviceIoControl(hVol, FSCTL_ENUM_USN_DATA, &med, sizeof(med),
		pData, sizeof(pData), &cb, NULL) != FALSE)
	{

		PUSN_RECORD pRecord = (PUSN_RECORD) &pData[sizeof(USN)];
		while ((PBYTE) pRecord < (pData + cb))
		{
			memset(fileName,0,MAX_PATH*2);
			WideCharToMultiByte(CP_OEMCP,NULL,pRecord->FileName,pRecord->FileNameLength/2,
				fileName,pRecord->FileNameLength,NULL,FALSE);  

			USNDirFileTreeAdd(pRecord-> FileReferenceNumber,
				pRecord->ParentFileReferenceNumber,
				pRecord->FileAttributes,
				fileName,filetreetotal,
				filetreemax,
				filetree,
				dirtreetotal,
				dirtreemax,
				dirtree);

			pRecord = (PUSN_RECORD) ((PBYTE) pRecord + pRecord->RecordLength);
		}
		med.StartFileReferenceNumber = * (DWORDLONG *) pData;
	}
}
//传入 "c:","NTFS"
void USNBuildFullPathTest(char *Rootname,char *NtfsStr)
{
	Rootname="C:";
	NtfsStr="NTFS";

	unsigned long long filetreetotal=0;
	unsigned long long filetreemax=0;
	struct USNFileTree *filetree=NULL;
	unsigned long long dirtreetotal=0;
	unsigned long long dirtreemax=0;
	struct USNDirTree *dirtree=NULL;

	if(IsNtfs(NtfsStr))
	{
		HANDLE hVol;
		if(GetVolumesHandle(Rootname[0],&hVol))
		{
			if(OpenUSNJournal(hVol))
			{
				USN_JOURNAL_DATA UsnInfo;
				if(QueryUSNJournal(hVol,&UsnInfo))
				{
					NTFS_VOLUME_DATA_BUFFER ntfsVolData;
					QueryUSNJournalVolume(hVol, &ntfsVolData);

					GetUSNJournalTest(hVol,UsnInfo,ntfsVolData,
						&filetreetotal,
						&filetreemax,
						&filetree,
						&dirtreetotal,
						&dirtreemax,
						&dirtree);

					USNBuildDirTree(ROOT_FileReferenceNumber,Rootname,dirtreetotal,dirtree);
					USNBuildFileTree(ROOT_FileReferenceNumber,Rootname,filetreetotal,filetree,dirtreetotal,dirtree);
				}

				ColseUSNJournal(hVol,UsnInfo);
			}
		}
	}

	FILE *fp=fopen("aaa.txt","wb");

	unsigned long long i;

	for( i=0;i<dirtreetotal;i++)
	{
		fprintf(fp,"%I64u,%I64u,%s\n",
			dirtree[i].ParentFileReferenceNumber,dirtree[i].FileReferenceNumber,
			dirtree[i].path);
	}
	fclose(fp);

	fp=fopen("bbb.txt","wb");

	for( i=0;i<filetreetotal ;i++)
	{
		fprintf(fp,"%I64u,%I64u,%s\n",
			filetree[i].ParentFileReferenceNumber,filetree[i].FileReferenceNumber,
			filetree[i].path);
	}
	fclose(fp);

	if(filetree)
		free(filetree);
	if(dirtree)
		free(dirtree);
}



bool ColseUSNJournal(HANDLE hVol,USN_JOURNAL_DATA UsnInfo)
{
	DELETE_USN_JOURNAL_DATA dujd;  
	DWORD br;  
	dujd.UsnJournalID = UsnInfo.UsnJournalID;  
	dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;  

	int status = DeviceIoControl(hVol,  
		FSCTL_DELETE_USN_JOURNAL,  
		&dujd,  
		sizeof (DELETE_USN_JOURNAL_DATA),  
		NULL,  
		0,  
		&br,  
		NULL);  

	if(0!=status)
		return true;
	else
		return false;
}
///////////////////////////////////////////////////////////////////

//数据插入
void OUT_USNFileTreeAdd( DWORDLONG FileReferenceNumber,
						DWORDLONG ParentFileReferenceNumber,
						char *name,
						unsigned long long *filetreetotal,
						unsigned long long *filetreemax,
struct USNFileTree **filetree,
	int maintype,
	char *fileext)
{
	unsigned long long i;

	i=(*filetreetotal);
	if(0==i)
	{
		(*filetreemax)=2000;
		(*filetree)=(struct USNFileTree *)calloc(	(*filetreemax),sizeof(struct USNFileTree));
	}
	if(i>=	(*filetreemax))
	{
		(*filetreemax)+=500;
		(*filetree)=(struct USNFileTree *)realloc((*filetree),	(*filetreemax)*sizeof(struct USNFileTree));
	}
	(*filetree)[i].ParentFileReferenceNumber=ParentFileReferenceNumber;
	(*filetree)[i].FileReferenceNumber=FileReferenceNumber;
	strcpy((*filetree)[i].name,name);

	(*filetree)[i].maintype=maintype;
	strcpy((*filetree)[i].fileext,fileext);

	(*filetreetotal)++;

}
void OUT_USNDirTreeAdd( DWORDLONG FileReferenceNumber,
					   DWORDLONG ParentFileReferenceNumber,
					   char *name,
					   unsigned long long *dirtreetotal,
					   unsigned long long *dirtreemax,
struct USNDirTree **dirtree)
{
	unsigned long long i;

	i=(*dirtreetotal);

	if(0== i)
	{
		(*dirtreemax)=2000;
		(*dirtree)=(struct USNDirTree *)calloc((*dirtreemax),sizeof(struct USNDirTree));
	}
	if(i>=(*dirtreemax))
	{
		(*dirtreemax)+=500;
		(*dirtree)=(struct USNDirTree *)realloc((*dirtree),(*dirtreemax)*sizeof(struct USNDirTree));
	}
	(*dirtree)[i].ParentFileReferenceNumber=ParentFileReferenceNumber;
	(*dirtree)[i].FileReferenceNumber=FileReferenceNumber;
	strcpy((*dirtree)[i].path,name);
	(*dirtree)[i].flag=false;
	(*dirtreetotal)++;
}

//遍历获取文件夹名 文件名 本节点 父节点编号
void OUT_GetUSNJournal(HANDLE hVol,USN_JOURNAL_DATA UsnInfo,
					   unsigned long long *filetreetotal,
					   unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree,
	list<struct FILETYPE_ST> typeList)

{
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;
	med.HighUsn = UsnInfo.NextUsn;

	// Process MFT in 64k chunks
	//BYTE pData[sizeof(DWORDLONG) + 0x10000];
	BYTE *pData=(BYTE *)calloc(sizeof(DWORDLONG) + 0x10000,sizeof(BYTE));

	DWORD cb;
	char fileName[MAX_PATH*2] = {0};  
	char fileext[MAX_PATH*2] = {0}; 
	int maintype;

	char firstExt[256]="";
	int extmaxlen=0;
	BuildFirstExt(typeList,firstExt,&extmaxlen);

	while (DeviceIoControl(hVol, FSCTL_ENUM_USN_DATA, &med, sizeof(med),
		pData, sizeof(DWORDLONG) + 0x10000, &cb, NULL) != FALSE)
	{

		PUSN_RECORD pRecord = (PUSN_RECORD) &pData[sizeof(USN)];
		while ((PBYTE) pRecord < (pData + cb))
		{
			memset(fileName,0,MAX_PATH*2);
			WideCharToMultiByte(CP_OEMCP,NULL,pRecord->FileName,pRecord->FileNameLength/2,
				fileName,pRecord->FileNameLength,NULL,FALSE);  
			if(pRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{	
				OUT_USNDirTreeAdd(pRecord-> FileReferenceNumber,
					pRecord->ParentFileReferenceNumber,
					fileName,
					dirtreetotal,
					dirtreemax,
					dirtree);
			}
			else
			{	
				memset(fileext,0,MAX_PATH*2);
				//先过滤一次 插文件
				GetFileExtStr(fileName,fileext);

				if(checkFirstExt(extmaxlen,firstExt,fileext))
				{
					//有效后缀
					maintype=checkExt(typeList,fileext);

					if(maintype>=0)
					{
						OUT_USNFileTreeAdd(pRecord-> FileReferenceNumber,
							pRecord->ParentFileReferenceNumber,
							fileName,
							filetreetotal,
							filetreemax,
							filetree, 
							maintype,
							fileext);
					}
				}
			}

			pRecord = (PUSN_RECORD) ((PBYTE) pRecord + pRecord->RecordLength);
		}
		med.StartFileReferenceNumber = * (DWORDLONG *) pData;
	}

	free(pData);
	pData=NULL;
}

//遍历所有文件USN模式
bool OUT_FindAllFile_USN(long long hdd_nid,char * hdd_area,
						 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList)
{
	unsigned long long i;

	unsigned long long filetreetotal=0;
	unsigned long long filetreemax=0;
	struct USNFileTree **filetree=(	struct USNFileTree **)calloc(1,sizeof(	struct USNFileTree *));
	unsigned long long dirtreetotal=0;
	unsigned long long dirtreemax=0;
	struct USNDirTree **dirtree=(	struct USNDirTree **)calloc(1,sizeof(	struct USNDirTree *));
	char Rootname[8]="";
	Rootname[0]=hdd_area[0];
	Rootname[1]=hdd_area[1];

	HANDLE hVol;
	if(GetVolumesHandle(hdd_area[0],&hVol))
	{
		if(OpenUSNJournal(hVol))
		{
			USN_JOURNAL_DATA UsnInfo;
			if(QueryUSNJournal(hVol,&UsnInfo))
			{
				OUT_GetUSNJournal(hVol,UsnInfo,
					&filetreetotal,
					&filetreemax,
					filetree,
					&dirtreetotal,
					&dirtreemax,
					dirtree,
					typeList);

			}
			else
			{
				ColseUSNJournal(hVol,UsnInfo);
				return false;
			}
			ColseUSNJournal(hVol,UsnInfo);
		}
		else
			return false;
	}
	else
		return false;


	USNBuildDirTree(ROOT_FileReferenceNumber,Rootname,dirtreetotal,(*dirtree));
	USNBuildFileTree(ROOT_FileReferenceNumber,Rootname,filetreetotal,(*filetree),dirtreetotal,(*dirtree));


	//可以把DIR释放了
	if((*dirtree))
	{
		free(*dirtree);
		(*dirtree)=NULL;
	}

	free(dirtree);
	dirtree=NULL;

	ULARGE_INTEGER  uli; 

	struct FILE_ST data;

	list<struct FILE_ST> Filedata;
	list<STR_SPLITE_S> strList;

	Filedata.clear();

	WIN32_FILE_ATTRIBUTE_DATA lpinf;

	for( i=0;i<filetreetotal;i++)
	{
		memset(&data,0,sizeof(struct FILE_ST ));

		strcpy(data.name,(*filetree)[i].name);
		strcpy(data.path,GetFilePathNoName((*filetree)[i].path));

		if(SQLDB.File_CheckDoublePos(data.name,data.path,hdd_nid))
			continue;

		data.hdd_nid=hdd_nid;
		strcpy(data.type,(*filetree)[i].fileext);

		//有效后缀
		data.maintype=(*filetree)[i].maintype;

		memset(&lpinf,0,sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		//获取文件信息，path为文件路径
		if(GetFileAttributesEx((*filetree)[i].path,GetFileExInfoStandard,&lpinf))
		{
			uli.LowPart = lpinf.ftCreationTime.dwLowDateTime;  
			uli.HighPart = lpinf.ftCreationTime.dwHighDateTime;  
			data.CreationTime=uli.QuadPart;

			uli.LowPart = lpinf.ftLastWriteTime.dwLowDateTime;  
			uli.HighPart = lpinf.ftLastWriteTime.dwHighDateTime;  
			data.LastWriteTime=uli.QuadPart;

			uli.LowPart = lpinf.nFileSizeLow;  
			uli.HighPart =lpinf.nFileSizeHigh;  
			data.filesize=uli.QuadPart;

		}
		if(MAINTYPE_OTHER == data.maintype )
		{
			strList.clear();
			StrSplite(strList,data.name);
			checkStr(data,zidianList,strList); 
			strList.clear();
		}
		else if( MAINTYPE_SUB == data.maintype)
		{
			NULL;
		}
		else  if(MAINTYPE_VIDEO == data.maintype )
		{
			if(systemset.c_noscan_video_min)
			{
				if(data.filesize<	((long long)(systemset.e_noscan_video_min)*1024*1024))
					continue;
			}

			if(	systemset.c_noscan_video_max)
			{
				if(data.filesize>	((long long)(systemset.e_noscan_video_max)*1024*1024))
					continue;
			}

#if OPEN_FFMEPG
			VideoPlay(filetree[i].path,
				&data.filetime,&data.resolutionW,&data.resolutionH);
#endif

			if(systemset.c_mediainfo)
			{
				VideoInfo((*filetree)[i].path,
					&data.filetime,&data.resolutionW,&data.resolutionH,
					&data.bitrateKbps);
			}

			strList.clear();
			StrSplite(strList,data.name);
			checkStr(data,zidianList,strList); 
			strList.clear();
		}
		else  if(MAINTYPE_MUSIC == data.maintype )
		{
			if(systemset.c_noscan_audio_min)
			{
				if(data.filesize<	((long long)(systemset.e_noscan_audio_min)*1024*1024))
					continue;
			}

			if(	systemset.c_noscan_audio_max)
			{
				if(data.filesize>	((long long)(systemset.e_noscan_audio_max)*1024*1024))
					continue;
			}

#if OPEN_FFMEPG
			AudioPlay(filetree[i].path,&data.filetime) ;
#endif
			if(systemset.c_mediainfo)
			{
				AudioInfo((*filetree)[i].path,
					&data.filetime,
					&data.bitrateKbps);
			}

			NULL;
		}

		Filedata.push_back(data);

		if(Filedata.size()>200)
		{	
			SQLDB.Begin();
			list<struct FILE_ST>::iterator beglist;

			for(beglist=Filedata.begin();beglist!=Filedata.end();beglist++)
			{
				SQLDB.File_Add(*beglist);
			}	
			SQLDB.Commit();
			//要放里面
			Filedata.clear();
		}

	}

	if(Filedata.size()>0)
	{	
		SQLDB.Begin();
		list<struct FILE_ST>::iterator beglist;

		for(beglist=Filedata.begin();beglist!=Filedata.end();beglist++)
		{
			SQLDB.File_Add(*beglist);
		}	
		SQLDB.Commit();

	}
	Filedata.clear();

	if((*filetree))
	{
		free(*filetree);
		(*filetree)=NULL;
	}

	free(filetree);
	filetree=NULL;

	return true;
}
