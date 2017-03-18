#include "stdafx.h"
#include "video.h"
#include "CService.h"
#include "SignalDownload.h"

#include "rhashinclude\rhash.h"
extern CSqliteOperate SQLDB;
extern TCHAR CurrentDir[MAX_PATH*2];
extern struct SYSTEM_SET_ST systemset;

//#include "SHELLAPI.H"

#if OPEN_FFMEPG
#ifdef __cplusplus
extern "C" 
{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
}
#endif
int VideoPlay(TCHAR* filePath,long *sec,int *w,int *h) 
{
	AVFormatContext *pFormatCtx=NULL;
	unsigned int             i;
	int videoStream;
	AVCodecContext  *pCodecCtx=NULL;

	// Register all formats and codecs
	//av_register_all();
	// Open video file
	if(av_open_input_file(&pFormatCtx, filePath, NULL, 0, NULL)!=0)
		return -1; // Couldn't open file

	// Retrieve stream information
	if(av_find_stream_info(pFormatCtx)<0)
	{
		av_close_input_file(pFormatCtx);
		pFormatCtx=NULL;
		pCodecCtx=NULL;
		return -1; // Couldn't find stream information
	}

	// Dump information about file onto standard error
	dump_format(pFormatCtx, 0, filePath, 0);
	// Find the first video stream
	videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) 
		{
			videoStream=i;
			break;
		}
	}

	if(videoStream==-1)
	{
		av_close_input_file(pFormatCtx);
		pFormatCtx=NULL;
		pCodecCtx=NULL;
		return -1; // Didn't find a video stream
	}
	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;

	if(0==pCodecCtx->width || 0==pCodecCtx->height)
	{
		av_close_input_file(pFormatCtx);
		pFormatCtx=NULL;
		pCodecCtx=NULL;
		fprintf(stderr, "codec w h unkown!\n");
		return -1;
	}
	/*
	// 计算总时长
	unsigned long long tns, thh, tmm, tss,us;
	//秒
	tns  = pFormatCtx->duration / AV_TIME_BASE;
	//微秒
	us = pFormatCtx->duration % AV_TIME_BASE * 100 / AV_TIME_BASE;
	//时
	thh  = tns / 3600;
	//分
	tmm  = (tns % 3600) / 60;
	//秒
	tss  = (tns % 60);
	*/

	(*sec)=pFormatCtx->duration / AV_TIME_BASE;
	(*w)=pCodecCtx->width ;
	(*h)=pCodecCtx->height;


	// Close the video file
	av_close_input_file(pFormatCtx);

	pFormatCtx=NULL;
	pCodecCtx=NULL;

	return 0;
}

void VideoInit(void)
{
	av_register_all();
}
int AudioPlay(TCHAR* filePath,long *sec) 
{
	AVFormatContext *pFormatCtx=NULL;
	// Register all formats and codecs
	//av_register_all();
	// Open video file
	if(av_open_input_file(&pFormatCtx, filePath, NULL, 0, NULL)!=0)
		return -1; // Couldn't open file

	// Retrieve stream information
	if(av_find_stream_info(pFormatCtx)<0)
	{
		av_close_input_file(pFormatCtx);
		pFormatCtx=NULL;

		return -1; // Couldn't find stream information
	}

	// Dump information about file onto standard error
	dump_format(pFormatCtx, 0, filePath, 0);
	/*
	// 计算总时长
	unsigned long long tns, thh, tmm, tss,us;
	//秒
	tns  = pFormatCtx->duration / AV_TIME_BASE;
	//微秒
	us = pFormatCtx->duration % AV_TIME_BASE * 100 / AV_TIME_BASE;
	//时
	thh  = tns / 3600;
	//分
	tmm  = (tns % 3600) / 60;
	//秒
	tss  = (tns % 60);
	*/

	(*sec)=pFormatCtx->duration / AV_TIME_BASE;

	// Close the video file
	av_close_input_file(pFormatCtx);

	pFormatCtx=NULL;

	return 0;
}


#endif

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

int VideoInfo(TCHAR* filePath,long *sec,int *w,int *h,long *bitrateKB)
{
	String a;
	long long b;
	MediaInfo MI;

	b=MI.Open(filePath);
	if(1!=b)
		return -1;

	a=MI.Get(Stream_Video, 0, __T("Duration"), Info_Text, Info_Name).c_str();

	if(a.length()>0)
	{
		b=_atoi64(a.c_str());
		(*sec)=b/1000;
	}
	else
		(*sec)=0;

	a=MI.Get(Stream_Video, 0, __T("Width"), Info_Text, Info_Name).c_str();
	if(a.length()>0)
	{
		(*w)=atoi(a.c_str());
	}
	else
		(*w)=0;

	a=MI.Get(Stream_Video, 0, __T("Height"), Info_Text, Info_Name).c_str();
	if(a.length()>0)
	{
		(*h)=atoi(a.c_str());
	}
	else
		(*h)=0;

	a=MI.Get(Stream_General, 0, __T("OverallBitRate"), Info_Text, Info_Name).c_str();
	if(a.length()>0)
	{
		b=_atoi64(a.c_str());
		(*bitrateKB)=b/1000;
	}
	else
		(*bitrateKB)=0;

	MI.Close();

	return 0;
}
int AudioInfo(TCHAR* filePath,long *sec,long *bitrateKB) 
{
	MediaInfo MI;
	String a;
	long long b;

	b=MI.Open(filePath);
	if(1!=b)
		return -1;

	a=MI.Get(Stream_Audio, 0, __T("Duration"), Info_Text, Info_Name).c_str();
	if(a.length()>0)
	{
		b=_atoi64(a.c_str());
		(*sec)=b/1000;
	}
	else
		(*sec)=0;

	a=MI.Get(Stream_Audio, 0, __T("BitRate"), Info_Text, Info_Name).c_str();
	if(a.length()>0)
	{
		b=_atoi64(a.c_str());
		(*bitrateKB)=b/1000;
	}
	else
		(*bitrateKB)=0;

	MI.Close();

	return 0;
}


long long filesize(TCHAR* filePath)
{
	FILE *fp;

	long long s=-1;
	fp=fopen(filePath,"rb");
	if(fp)
	{
		_fseeki64(fp,0,SEEK_END );
		s=_ftelli64(fp);
		fclose(fp);
	}
	return s;
}

void CHAR2UTF8(const char *str,char *utf8)
{
	if(str)
	{
		unsigned long int len=strlen(str);
		wchar_t * wbuff=(wchar_t *)calloc(len*2+4/*+4*/,sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, str, -1, wbuff, len+2);  
		// buffLen = WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, NULL, 0, 0, 0);  
		//  utf8 = new char[buffLen+1];  
		WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, (LPSTR)utf8, len*2+4, 0, 0); 
		free(wbuff);
		wbuff=NULL;
	}

}

void UTF82CHAR(const char *utf8,char *str)
{

	if(utf8)
	{
		unsigned long int len=strlen(utf8);
		wchar_t * wbuff=(wchar_t *)calloc(len*2+4,sizeof(wchar_t));
		//	int buffLen = 0;  
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuff, len+2);  
		//	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);  
		//	m_gb2312 = new char[buffLen+1];  
		WideCharToMultiByte(CP_ACP, 0, wbuff, -1, (LPSTR)str, len*2+4, 0, 0);  
		free(wbuff);
		wbuff=NULL;
	}

}

void UTF82CHAR_SQL(char *name,int argc, char **argv, char **ColName,char *str)
{
	int i=0;
	for(i=0;i<argc;i++)
	{
		if(0==strcmp(name,ColName[i]))
			break;
	}

	if(argv[i])
	{
		unsigned long int len=strlen(argv[i]);
		if(len>3000)
			return ;
		wchar_t * wbuff=(wchar_t *)calloc(len*2+4,sizeof(wchar_t));
		//	int buffLen = 0;  
		MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, wbuff, len+2);  
		//	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);  
		//	m_gb2312 = new char[buffLen+1];  
		WideCharToMultiByte(CP_ACP, 0, wbuff, -1, (LPSTR)str, len*2+4, 0, 0);  
		free(wbuff);
		wbuff=NULL;
	}

}


long UTF82LONG(char *name,int argc, char **argv, char **ColName)
{
	int i=0;
	for(i=0;i<argc;i++)
	{
		if(0==strcmp(name,ColName[i]))
			break;
	}

	if(argv[i])
	{
		if(strlen(argv[i])>0)
		{
			long a;
			sscanf(argv[i],"%d",&a);
			return a;
		}
	}
	return 0;
}

long long UTF82LONGLONG(char *name,int argc, char **argv, char **ColName)
{
	int i=0;
		for(i=0;i<argc;i++)
		{
			if(0==strcmp(name,ColName[i]))
				break;
		}

	if(argv[i])
	{
		if(strlen(argv[i])>0)
		{
			long long a;
			sscanf(argv[i],"%I64u",&a);
			return a;
		}
	}
	return 0;

}

//将字符串放到剪切板
int AppendBufferToClipboard( char *cBuffer, unsigned long nSize )
{
	HGLOBAL hClipboard = NULL;
	char *pClipBuf = NULL;

	if( OpenClipboard(NULL) && 	EmptyClipboard() )
	{ 
		hClipboard = GlobalAlloc( GMEM_MOVEABLE, nSize+2 );
		pClipBuf = (char*)GlobalLock(hClipboard);
		memset( pClipBuf, 0, nSize+2 );
		memcpy( pClipBuf, cBuffer, nSize+2 );
		GlobalUnlock( hClipboard );
		SetClipboardData( CF_OEMTEXT, hClipboard );
		CloseClipboard();

	}
	else
	{
		return -1;
	}

	return 0;
}
//从剪切板获取字符串
int GetBufferFromClipboard( char *cBuffer, unsigned long  nMaxSize )
{
	unsigned long nNeedSize = 0;
	HANDLE hClipboard = NULL;
	char *pClipBuf = NULL;

	if ( OpenClipboard( NULL ) )
	{
		hClipboard = GetClipboardData( CF_TEXT );
		nNeedSize = GlobalSize( hClipboard );
		if ( nNeedSize > nMaxSize+1 )
		{
			return -1;
		}

		pClipBuf = (char*)GlobalLock( hClipboard );
		memset( cBuffer, 0, nMaxSize );
		memcpy( cBuffer, pClipBuf, nNeedSize );
		GlobalUnlock( hClipboard );
		CloseClipboard();

		return nNeedSize;

	}
	else
	{
		return 0;
	}


}
//清空剪切板
int ClearClipboradBuffer(void)
{
	HGLOBAL hClipboard = NULL;

	if( OpenClipboard( NULL ) )
	{ 
		//hClipboard = GetClipboardData( CF_TEXT );
		//GlobalFree( hClipboard );
		EmptyClipboard();
		CloseClipboard();
		return 0;
	}
	else
	{
		return -1;
	}
}

//获取文件后缀,并变小写
CString GetFileExt(CString fileName)
{
	return (fileName.Right(fileName.GetLength()-fileName.ReverseFind('.'))).MakeLower();
}

void GetFileExtStr(CString fileName,char *fileext)
{
	strcpy(fileext,(fileName.Right(fileName.GetLength()-fileName.ReverseFind('.'))).MakeLower());
}

//获取文件相对路径。去除盘符和名字
CString GetFilePathNoName(CString filepath)
{
	CString path=filepath.Right(filepath.GetLength()-filepath.Find('\\')-1);
	return path.Left(path.ReverseFind('\\')+1);

}
//比较后缀
int checkExt(list<struct FILETYPE_ST> typeList,char *fileext) 
{
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->enable)
		{
			if(  0== strcmp(beglist->type,fileext) )
			{
				return beglist->maintype ;
			}
		}
	}
	// -1为无效
	return -1;
}
//提取后缀的第一个字符
//得出来的firstExt 是不重复的扩展名首字母。
void BuildFirstExt(list<struct FILETYPE_ST> typeList,char *firstExt,int *maxlen) 
{
	int i=0;
	int j=0;
	(*maxlen)=0;
	list<struct FILETYPE_ST>::iterator beglist;

	for(beglist=typeList.begin();beglist!=typeList.end();beglist++)
	{
		if(beglist->enable)
		{
			if(0!=i)
			{
				for(j=0;j<i;j++)
				{
					if(	firstExt[j]==beglist->type[1])
					{
						break;
					}
				}
				if(j==i)
				{
					firstExt[i]=beglist->type[1];
					i++;
				}
			}
			else
			{
				firstExt[i]=beglist->type[1];
				i++;
			}

			if((*maxlen)<strlen(beglist->type))
				(*maxlen)=strlen(beglist->type);
		}
	}
}

bool checkFirstExt(int maxlen,char *firstExt,char *fileext) 
{
	if(strlen(fileext)>maxlen)
		return false;

	while(*firstExt)
	{
		if( (*firstExt)==fileext[1])
			return true;

		firstExt++;
	}

	return false;
}


//分割字符串
void StrSplite(list<struct STR_SPLITE_S> &strList,char *src) 
{
	struct STR_SPLITE_S a={0};
	int i=0;
	strList.clear();

	while(*src)
	{
		if(' '==(*src) || '.'==(*src) || '-'==(*src) || '_'==(*src)|| '+'==(*src))
		{
			//小写
			strlwr(a.s);
			strList.push_back(a);
			memset(&a,0,sizeof(struct STR_SPLITE_S));
			i=0;
		}
		else
		{
			a.s[i]=(*src);
			i++;
		}
		src++;

	}
	strlwr(a.s);
	strList.push_back(a);
}

//比较字符串
void checkStr(struct FILE_ST &data,list<struct ZIDIAN_ST> zidianList,list<struct STR_SPLITE_S> strList) 
{
	list<struct STR_SPLITE_S>::iterator beglistA;
	list<struct ZIDIAN_ST>::iterator beglistB;

	for(beglistA=strList.begin();beglistA!=strList.end();beglistA++)
	{
		if(strlen(beglistA->s) <=16)
		{
			for(beglistB=zidianList.begin();beglistB!=zidianList.end();beglistB++)
			{
				if(0==strcmp(beglistA->s,beglistB->lowzidian))
				{
					if(ZIDIAN_YAZHI == beglistB->mainzidian)
					{
						strcpy(data.zidian_yazhi,beglistB->zidian);
					}
					else if(ZIDIAN_HUAZHI == beglistB->mainzidian)
					{
						strcpy(data.zidian_huazhi,beglistB->zidian);
					}
					else if(ZIDIAN_FENBIANLV == beglistB->mainzidian)
					{
						strcpy(data.zidian_fenbianlv,beglistB->zidian);
					}
					else if(ZIDIAN_3D == beglistB->mainzidian)
					{
						strcpy(data.zidian_3d,beglistB->zidian);
					}
					else if(ZIDIAN_YEAR == beglistB->mainzidian)
					{
						strcpy(data.zidian_year,beglistB->zidian);
					}
					break;
				}
			}
		}
	}

}


//遍历所有文件
void FindAllFile(long long hdd_nid,CString hdd_area,
				 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList)
{

	FILETIME cTime;
	FILETIME lTime;
	CString fileName;
	CString fileExt;

	ULARGE_INTEGER  uli; 

	struct FILE_ST data;
	list<STR_SPLITE_S> strList;

	CFileFind fileFinder;
	CString filePath = hdd_area + _T("//*.*");

	BOOL bFinished = fileFinder.FindFile(filePath);

	while(bFinished)  //每次循环对应一个类别目录
	{
		bFinished = fileFinder.FindNextFile();

		if(fileFinder.IsDirectory() && !fileFinder.IsDots())  //若是目录则递归调用此方法
		{
			FindAllFile(hdd_nid,fileFinder.GetFilePath(),typeList,zidianList);
		}
		else  //再判断是否为txt文件
		{
			//获取文件类型
			fileName = fileFinder.GetFileName();
			fileExt=GetFileExt(fileName);

			memset(&data,0,sizeof(struct FILE_ST ));
			//有效后缀
			data.maintype=checkExt(typeList,fileExt.GetBuffer(0));

			if(data.maintype >=0)
			{
				if(SQLDB.File_CheckDoublePos(fileName.GetBuffer(0),
					GetFilePathNoName(fileFinder.GetFilePath()).GetBuffer(0),
					hdd_nid))
					continue;

				strcpy(data.name,fileName.GetBuffer(0)	);
				strcpy(data.path,GetFilePathNoName(fileFinder.GetFilePath()).GetBuffer(0));

				data.hdd_nid=hdd_nid;
				strcpy(data.type,fileExt.GetBuffer(0));
				data.filesize=fileFinder.GetLength();

				if(fileFinder.GetCreationTime(&cTime))
				{
					uli.LowPart = cTime.dwLowDateTime;  
					uli.HighPart = cTime.dwHighDateTime;  
					data.CreationTime=uli.QuadPart;
				}
				else
					data.CreationTime=0;

				if(fileFinder.GetLastWriteTime(&lTime))
				{
					uli.LowPart = lTime.dwLowDateTime;  
					uli.HighPart = lTime.dwHighDateTime;  
					data.LastWriteTime=uli.QuadPart;
				}
				else 
					data.LastWriteTime=0;

			}
			if(MAINTYPE_OTHER == data.maintype )
			{
				strList.clear();
				StrSplite(strList,fileName.GetBuffer(0));
				checkStr(data,zidianList,strList); 
				strList.clear();
			}
			else if( MAINTYPE_SUB == data.maintype)
			{
				NULL;
			}
			else  if(MAINTYPE_VIDEO == data.maintype )
			{
#if OPEN_FFMEPG
				VideoPlay(fileFinder.GetFilePath().GetBuffer(0),
					&data.filetime,&data.resolutionW,&data.resolutionH);
#endif
				strList.clear();
				StrSplite(strList,fileName.GetBuffer(0));
				checkStr(data,zidianList,strList); 
				strList.clear();
			}
			else  if(MAINTYPE_MUSIC == data.maintype )
			{
#if OPEN_FFMEPG
				AudioPlay(fileFinder.GetFilePath().GetBuffer(0),&data.filetime) ;
#endif

				NULL;
			}
			if(data.maintype >=0)
			{
				SQLDB.File_Add(data);
			}
		}
	}

	fileFinder.Close();
}

//遍历所有文件
void FindAllFile_NEW(long long hdd_nid,CString hdd_area,
					 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList)
{

	FILETIME cTime;
	FILETIME lTime;
	CString fileName;
	CString fileExt;

	ULARGE_INTEGER  uli; 

	struct FILE_ST data;

	list<struct FILE_ST> Filedata;
	list<STR_SPLITE_S> strList;

	CFileFind fileFinder;
	CString filePath = hdd_area + _T("//*.*");

	BOOL bFinished = fileFinder.FindFile(filePath);

	char firstExt[256]="";
	int extmaxlen=0;
	BuildFirstExt(typeList,firstExt,&extmaxlen);

	Filedata.clear();
	while(bFinished)  //每次循环对应一个类别目录
	{
		bFinished = fileFinder.FindNextFile();

		if(fileFinder.IsDots())
			continue;
		else if(fileFinder.IsDirectory())  //若是目录则递归调用此方法
		{
			FindAllFile_NEW(hdd_nid,fileFinder.GetFilePath(),typeList,zidianList);
		}
		else  //再判断是否为txt文件
		{
			//获取文件类型
			fileName = fileFinder.GetFileName();
			fileExt=GetFileExt(fileName);

			//有效后缀
			if(false ==checkFirstExt(extmaxlen,firstExt,fileExt.GetBuffer(0)))
				continue;

			memset(&data,0,sizeof(struct FILE_ST ));
			//有效后缀
			data.maintype=checkExt(typeList,fileExt.GetBuffer(0));

			if(data.maintype >=0)
			{
				strcpy(data.name,fileName.GetBuffer(0)	);
				strcpy(data.path,GetFilePathNoName(fileFinder.GetFilePath()).GetBuffer(0));

				if(SQLDB.File_CheckDoublePos(data.name,
					data.path,
					hdd_nid))
					continue;

				data.hdd_nid=hdd_nid;
				strcpy(data.type,fileExt.GetBuffer(0));
				data.filesize=fileFinder.GetLength();

				if(fileFinder.GetCreationTime(&cTime))
				{
					uli.LowPart = cTime.dwLowDateTime;  
					uli.HighPart = cTime.dwHighDateTime;  
					data.CreationTime=uli.QuadPart;
				}
				else
					data.CreationTime=0;

				if(fileFinder.GetLastWriteTime(&lTime))
				{
					uli.LowPart = lTime.dwLowDateTime;  
					uli.HighPart = lTime.dwHighDateTime;  
					data.LastWriteTime=uli.QuadPart;
				}
				else 
					data.LastWriteTime=0;

			}
			if(MAINTYPE_OTHER == data.maintype )
			{
				strList.clear();
				StrSplite(strList,fileName.GetBuffer(0));
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
				VideoPlay(fileFinder.GetFilePath().GetBuffer(0),
					&data.filetime,&data.resolutionW,&data.resolutionH);
#endif
				if(systemset.c_mediainfo)
				{
					VideoInfo(fileFinder.GetFilePath().GetBuffer(0),
						&data.filetime,&data.resolutionW,&data.resolutionH,
						&data.bitrateKbps);
				}

				strList.clear();
				StrSplite(strList,fileName.GetBuffer(0));
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
				AudioPlay(fileFinder.GetFilePath().GetBuffer(0),&data.filetime) ;
#endif
				if(systemset.c_mediainfo)
				{
					AudioInfo(fileFinder.GetFilePath().GetBuffer(0),
						&data.filetime,
						&data.bitrateKbps);
				}
				NULL;
			}
			if(data.maintype >=0)
			{
				Filedata.push_back(data);
			}
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

	fileFinder.Close();
}


void CheckAllDoubleFile(long long hdd_nid,char *path)
{
	long long doublenid;
	CString strid;
	char cstrid[64];

	list<struct FILE_VIEW_LITE_ST> fileviewList;
	list<struct FILE_VIEW_LITE_ST>::iterator beglist;
	list<struct FILE_VIEW_LITE_ST> fileviewDoubleList;
	list<struct FILE_VIEW_LITE_ST>::iterator Doublebeglist;
	fileviewList.clear();
	fileviewDoubleList.clear();

	if(SQLDB.File_ReadHddAllwithVideoAudio(fileviewList,hdd_nid,path))
	{
		for(beglist=fileviewList.begin();beglist!=fileviewList.end();beglist++)
		{
			fileviewDoubleList.clear();
			SQLDB.File_CheckDouble(beglist->file_maintype,beglist->filesize,
				beglist->file_name,beglist->file_md5,
				fileviewDoubleList);

			//查看重复 必须多于1条
			if(fileviewDoubleList.size()>1)
			{
				//发现重复的
				doublenid=0;
				//看看之前有没ID。 复用他
				for(Doublebeglist=fileviewDoubleList.begin();Doublebeglist!=fileviewDoubleList.end();Doublebeglist++)
				{
					if(Doublebeglist->double_nid>0)
					{
						doublenid=Doublebeglist->double_nid;
						break;
					}
				}
				//新增ID
				if(doublenid <=0 )
				{
					SQLDB.Double_Add(beglist->file_nid);
					//获取一个DOUBLEID 
					SQLDB.Double_ReadOne(&doublenid,beglist->file_nid);
				}
				//写到各个文件里
				if(doublenid >0)	
				{
					strid="";
					for(Doublebeglist=fileviewDoubleList.begin();Doublebeglist!=fileviewDoubleList.end();Doublebeglist++)
					{
						if(Doublebeglist!=fileviewDoubleList.begin())
							sprintf(cstrid,",%I64u",Doublebeglist->file_nid);
						else
							sprintf(cstrid,"%I64u",Doublebeglist->file_nid);
						strid+=cstrid;
					}

					SQLDB.File_SetDouble(doublenid,strid.GetBuffer(0));
				}
			}

		}
	}
	fileviewList.clear();
	fileviewDoubleList.clear();

}
void CheckAllIsFile(long long hdd_nid,char *path)
{
	FILE *fp=NULL;
	char str[512];

	list<struct FILE_VIEW_LITE_ST> fileviewList;
	list<struct FILE_VIEW_LITE_ST>::iterator beglist;

	fileviewList.clear();
	//查看文件是否存在
	if(SQLDB.File_ReadHddAllandOld(fileviewList,hdd_nid,path))
	{
		for(beglist=fileviewList.begin();beglist!=fileviewList.end();beglist++)
		{
			sprintf(str,"%s%s%s",beglist->hdd_area,beglist->file_path,beglist->file_name);
			fp=fopen(str,"rb");
			if(fp)
			{
				//文件存在
				fclose(fp);
				fp=NULL;
			}
			else
			{
				//文件不存在
				SQLDB.File_Delete(beglist->file_nid);
			}
		}
	}
	//重置全部文件为0
	SQLDB.File_SetNoFlagOne(hdd_nid);
	fileviewList.clear();

}

//生成ED2K连接。
void GetEd2kUrl(char* path,char *name,char *dst)
{
	unsigned char digest[64]="";
	char outputed2k[256]="";
	char outputaich[256]="";

	strcpy(dst,"");
	long long sizemax=1024*1024;
	sizemax*=1024*4;

	long long size=filesize(path);
	if(size >= sizemax)
	{
		//大于4G 退出
		return ;
	}
	rhash_library_init(); 

	rhash_file(RHASH_ED2K,path, digest);
	rhash_print_bytes(outputed2k, digest, rhash_get_digest_size(RHASH_ED2K),(RHPR_HEX));


	rhash_file(RHASH_AICH,path, digest);
	rhash_print_bytes(outputaich, digest, rhash_get_digest_size(RHASH_AICH),(RHPR_BASE32 ));

	sprintf(dst,"ed2k://|file|%s|%I64u|%s|h=%s",name,size,outputed2k,outputaich);

	//	rhash_file(RHASH_TTH,filepath, digest);
	//	rhash_print_bytes(output, digest, rhash_get_digest_size(RHASH_TTH),(RHPR_BASE32 ));

	//	rhash_file(RHASH_SHA1,filepath, digest);
	//	rhash_print_bytes(output, digest, rhash_get_digest_size(RHASH_SHA1 ),RHPR_HEX);

}

void HashInit(void)
{
	rhash_library_init(); 
}
//生成ED2K连接。
void GetHASH(char* path,char *ed2k,char *aich,char *btih)
{
	unsigned char digest[64]="";

#if PUBILC_ED2K
	Sleep(1000);
#endif

	//rhash_library_init(); 
	rhash_file(RHASH_ED2K,path, digest);
	rhash_print_bytes(ed2k, digest, rhash_get_digest_size(RHASH_ED2K),(RHPR_HEX));

#if PUBILC_ED2K
	Sleep(1000);
#endif

	rhash_file(RHASH_AICH,path, digest);
	rhash_print_bytes(aich, digest, rhash_get_digest_size(RHASH_AICH),(RHPR_BASE32 ));

#if PUBILC_ED2K
	Sleep(1000);
#endif

//BTIH不能用
//	rhash_file(RHASH_BTIH,path, digest);
//	rhash_print_bytes(btih, digest, rhash_get_digest_size(RHASH_BTIH),(RHPR_HEX ));

}


//射手专用
bool SVsubDownload_ComputeFileHash(char *dst,char *filepath)
{

	long long filesize=0;
	long long offset[4];
	unsigned char bBuf[1024 * 4];

	unsigned char digest[64]="";
	char md5hex[256]="";

	rhash_library_init(); 

	FILE *fp;

	fp=fopen(filepath,"rb");
	if(fp)
	{
		_fseeki64(fp,0,SEEK_END );
		filesize=_ftelli64(fp);

		if( filesize < 8 * 1024)
		{
			fclose(fp);
			return false;
		}

		offset[3] =filesize - 8 * 1024;
		offset[2] =filesize/ 3;
		offset[1] =filesize/ 3 * 2;
		offset[0] = 4 * 1024;

		for(int i=0;i<4;i++)
		{
			_fseeki64(fp,offset[i],SEEK_SET );
			fread(bBuf,4*1024,1,fp);

			rhash_msg(RHASH_MD5,bBuf,4*1024, digest);
			rhash_print_bytes(md5hex, digest, rhash_get_digest_size(RHASH_MD5),(RHPR_HEX));
			strcat(dst,md5hex);
			strcat(dst,"%3b");// “;”的网络化
		}

		fclose(fp);
		return true;
	}
	return false;
}
/*
POST /api/subapi.php HTTP/1.1
Content-Type: application/x-www-form-urlencoded
Host: shooter.cn
Expect: 100-continue
Connection: Keep-Alive
Content-Length: 239
发
filehash=6faa15a2fd44c34ed0c78437d8191616%3bfae3fd0b26e52e5193b29dc49adaa2a6%3bc326e105b30873d9cec22e32094bff82%3b9a3d632763588aae0a75bf7d865022d3&pathinfo=D%3a%5cThe.Big.Bang.Theory.S08E14.720p.HDTV.X264-DIMENSION.mkv&format=json&lang=Chn
回
[{"Desc":"","Delay":0,"Files":[{"Ext":"srt","Link":"https://www.shooter.cn/api/subapi.php?fetch=MTQzMjI3NzQ3NXxzYkhITm9zZkdaSExCT1VwbEpmLWxYNU8tMFJfaTdKRkxiZUV3UHRYY082NjIySjV4R01xaVRDSnRpeHgzbHhUY3gxUkxla1VwMDR5N1RtRG0ySUpXUGR6alluMEh5RXF3MzRPMkU0NHB1NlRfNG9NdmM3QmMyM0dFV0Jxb0hTZGw0ZWZCb2hWcFF4MlQ2VzJPM19mNFZpMEU4NjVXLUI3d2FuME9WbnBaZE1meHVkZnyiHY8pdJF_tjpaWH4cOnVmj1QqG594h-TX-Bqxt7d0yw==\u0026nonce=G%17%D1v%8A%B9Z%90l%0E%A7%D0%B3%27%15%A8"}]},{"Desc":"","Delay":0,"Files":[{"Ext":"ass","Link":"https://www.shooter.cn/api/subapi.php?fetch=MTQzMjI3NzQ3NXxWem1Vamh5RXpja19xUERWT0pMQl9feFlKcFgzd01EWll0ckZ4UWpKMml3SkNXRHg3b0lMcE53UTh1LV93UVhpa3ZRV3VBdUdnUlJ5eWRGRDJLcVpBM2VVX0hwLWV5WkRxWFJtOWZmazBsbENQdlA5bDN1T2xSZy00UjhxWGEyTExwdnNxaDluTGNEM3EwdklYZmJGUE1xWE9aeDVWYi1ncTA3bUJOWWowZl9wM2tkdHxFCqnztc7jbXZOrnUxPBoeh1A0qz_IKn3cHx9CeNi6og==\u0026nonce=%A5sQ%E4%07%D5%9C%80b%BFyp%C3I%FA%9E"}]},{"Desc":"","Delay":0,"Files":[{"Ext":"srt","Link":"https://www.shooter.cn/api/subapi.php?fetch=MTQzMjI3NzQ3NXx1X05IaWFPQzh4YzNmbUxSZWJscUp6TE1PRGhXY3lveEdybVc0QVVaVndzTUU2TFg4allBQW9wTDBESUI0UF94TTlHM3pteUtpQVd4dUFhRFZMR1lpVGszSTk1aHVSVHBBWGEyLVFhSi1Da1RHVW9vRGhCSHA2dmtWTnlDR19PZ1VRcDN2N09pWnRtSlBZeE5wczg2Wlp5U0VjdFZLR1hUN1hVdXF3aUFOV2QtUmpvbXx6UY5Vi6p8Hlwa8YyPu8sv5bp7LYK6avTQ6209qcL0fQ==\u0026nonce=bd%B6%81%CD%FC%C6%E3_%FA%3B%5Ei%AA%DB%F0"}]}]
*/

int SVsubDownload_Post(char *out,char *md5str,char *Chn_eng)
{
	Service CService;

	CService.InitData();

	char url[256]="http://shooter.cn/api/subapi.php";

	char poststr[1024]="";

	sprintf(poststr,"filehash=%s&pathinfo=anyvideo.mkv&format=json&lang=%s",md5str,Chn_eng);

	int nRes = CService.HttpRequest("POST",url,poststr,strlen(poststr),"",0,0);

	strcpy(out,CService.m_resp_buffer.c_str());

	CService.DestroyData();

	return nRes;
}

bool SVsubDownload_Get(char *url,char *filepath) 
{
	// TODO: Add your control notification handler code here
	SignalDownload sd;
	sd.InitData();

	bool proxyflag=systemset.checkProxy;
	char fail[1024]="";

	if(!sd.HTTPDownload(url,true,filepath,proxyflag,systemset.Proxy,fail,5,0,false))
	{
		sd.DestroyData();
		printf("无法连接原因:%s\n",fail);
		return false;
	}
	//	memcpy(	filestr,sd.outstr,sd.totalstrlen);
	sd.DestroyData();

	return true;
}

//下载射手 ONLYONE=TRUE 仅下载一个字幕 下载全部
//CHN  =1下中文 =2下英文 =3中英都下
int SVsubDownload_Download(char *md5str,char *path,bool onlyone,int chn)
{
	char outstr[5120]="";
	char *p=NULL;
	char *src=NULL;
	int nres=-1;
	int i=0;

	CString pathOutExt=path;
	pathOutExt=pathOutExt.Left(pathOutExt.ReverseFind('.'));
	char subpath[512];

	if(chn & 0x01)
	{
		nres= SVsubDownload_Post(outstr,md5str,"Chn");
		if(0==nres)
		{
			if(strlen(outstr)>5)
			{
				p=NULL;
				src=outstr;
				i=0;
				while(p=strstr(src,"Ext"))
				{
					//获取后缀
					CString tempstr=p+6;
					int strp=tempstr.Find('\"');
					CString url=tempstr.Left(strp);

					sprintf(subpath,"%s.chs%d.%s",pathOutExt.GetBuffer(0),i,url);
					//获取URL
					p=strstr(p,"https://");
					tempstr=p;
					strp=tempstr.Find('\"');
					url=tempstr.Left(strp);
					src=p+strp;
					url.Replace("\\u0026nonce","&nonce");

					SVsubDownload_Get(url.GetBuffer(0),subpath);
					i++;
					if(onlyone)
						break;
				}
			}
			else
				nres=-1;
		}
	}

	if(chn &0x02)
	{
		memset(outstr,0,5120);
		nres= SVsubDownload_Post(outstr,md5str,"eng");
		if(0==nres)
		{
			if(strlen(outstr)>5)
			{
				p=NULL;
				src=outstr;
				i=0;
				while(p=strstr(src,"Ext"))
				{
					//获取后缀
					CString tempstr=p+6;
					int strp=tempstr.Find('\"');
					CString url=tempstr.Left(strp);

					sprintf(subpath,"%s.eng%d.%s",pathOutExt.GetBuffer(0),i,url);
					//获取URL
					p=strstr(p,"https://");
					tempstr=p;
					strp=tempstr.Find('\"');
					url=tempstr.Left(strp);
					src=p+strp;
					url.Replace("\\u0026nonce","&nonce");

					SVsubDownload_Get(url.GetBuffer(0),subpath);
					i++;
					if(onlyone)
						break;
				}
			}
			else
				nres=-1;
		}
	}


	return nres;
}

char ConvertStringStr[520];

char* ConvertString(char * strText)
{

	memset(ConvertStringStr,0,520);

	CString  strIniPath= CurrentDir;
	strIniPath +="//langchn.ini";

	GetPrivateProfileString("String",strText,"",ConvertStringStr,520, strIniPath);//"./langchn.ini"

	if(ConvertStringStr[0]==0)
	{
		return strText;
	}
	else
		return ConvertStringStr;

}

bool FileDeleteToUndo(char *path)
{
	char strTitle[]="文件删除到回收站"; //文件删除进度对话框标题
	//PATH后面必须有2个\0 所以在此要初始化 不然会出现源文件不存在的崩溃
	//MSDN上说要以两个\0结束，所以一般的字符串必须还要加上一个\0
	char path2[520]={0};
	sprintf(path2,"%s",path);

	SHFILEOPSTRUCT FileOp;//定义SHFILEOPSTRUCT结构对象;
	FileOp.hwnd=NULL;
	FileOp.wFunc=FO_DELETE; //执行文件删除操作;
	FileOp.pFrom=path2;
	FileOp.pTo=path2;
	FileOp.fFlags=FOF_ALLOWUNDO;//此标志使删除文件备份到Windows回收站
	FileOp.hNameMappings=NULL;
	FileOp.lpszProgressTitle=strTitle;
	//这里开始删除文件
	return (SHFileOperation(&FileOp) == 0) ;
}

//
//
//http://subtitle.kankan.xunlei.com:8000/search.json/mname=Fantastic.Four.2015.1080p.BluRay.x264-GECKOS
//
//
//{"sublist":[{"scid":"2B39B8882B679E72CC4310CCD55B0BED814B8654", "sname":"Fantastic.Four.2015.720P.HDTS.x264.AC3.HQ.Hive-CM8","sext":"srt","language":"简体&英语","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/2B\/39\/2B39B8882B679E72CC4310CCD55B0BED814B8654.srt"},
//{"scid":"2B35935BAE520D16774F93412859AFC9C28FF6AA", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG.chn1","sext":"srt","language":"简体&英语","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/2B\/35\/2B35935BAE520D16774F93412859AFC9C28FF6AA.srt"},
//{"scid":"EEF825FA02FED10A8386CFCAF445B34B914B6511", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG.mkv","sext":"srt","language":"简体","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/EE\/F8\/EEF825FA02FED10A8386CFCAF445B34B914B6511.srt"},
//{"scid":"3ED116BB2B1A7ED0BCA07C25ED1FC9B40ED9C9F4", "sname":"Fantastic.Four.2015.720P.HDTS.x264.AC3.HQ.Hive-CM8.mkv_1439364617","sext":"ass","language":"简体","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/3E\/D1\/3ED116BB2B1A7ED0BCA07C25ED1FC9B40ED9C9F4.ass"},
//{"scid":"0FE4D187D989654C7E72896156332FF22EE6FAEC", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG.chs","sext":"ssa","language":"简体","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/0F\/E4\/0FE4D187D989654C7E72896156332FF22EE6FAEC.ssa"},
//{"scid":"8E6AB100EDD601D4A6376F86CBD257D5C31AD60A", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG.chs&amp.英文","sext":"srt","language":"英语","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/8E\/6A\/8E6AB100EDD601D4A6376F86CBD257D5C31AD60A.srt"},
//{"scid":"BAA15DA267DA307828B7D4DE1FC596CA55315A0B", "sname":"Fantastic Four 2015 KORSUB 1080p HDRip x264 AAC-JYK.chs","sext":"ass","language":"简体","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/BA\/A1\/BAA15DA267DA307828B7D4DE1FC596CA55315A0B.ass"},
//{"scid":"58C7CCDB57DBDB4D7C1CD230C9F8819B50028F83", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG.chn","sext":"srt","language":"简体","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/58\/C7\/58C7CCDB57DBDB4D7C1CD230C9F8819B50028F83.srt"},
//{"scid":"7621AE62D8C9EE05678C867B27F691C5FE4B6235", "sname":"Fantastic.Four.2015.HC.HDRip.XViD.AC3-ETRG","sext":"srt","language":"英语","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/76\/21\/7621AE62D8C9EE05678C867B27F691C5FE4B6235.srt"},
//{"scid":"77C2B3312252D68C94D0CDD24CD87A43C8C3DEA0", "sname":"Fantastic.Four.2015.1080p.HDRip.KORSUB.x264.AAC2.0-RARBG","sext":"srt","language":"简体&英语","simility":"1","surl":"http:\/\/subtitle.v.geilijiasu.com\/77\/C2\/77C2B3312252D68C94D0CDD24CD87A43C8C3DEA0.srt"},
//{}]}
//
// -1网络失败 0无数据 1有数据
int GetXunleiSub(char *name,list<struct XUNLEI_SUB_ST> &subList)
{
	//清除
	subList.clear();

	SignalDownload sd;
	sd.InitData();

	char fail[1024]="";

	char str[520];
	sprintf(str,"http://subtitle.kankan.xunlei.com:8000/search.json/mname=%s",name);

	if(!sd.HTTPDownload(
		str,
		false,"",systemset.checkProxy,systemset.Proxy,fail,10,0,false))
	{
		sd.DestroyData();
		return -1;
	}
	struct XUNLEI_SUB_ST data;
	Json::Reader reader;  
	Json::Value root;  
	int file_size=0;
	if (reader.parse(sd.outstr, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	{  

		file_size= root["sublist"].size();  // 得到"sublist"的数组个数  
		if(1==file_size)
			return 0;

		file_size--;

		for(int i=0;i<file_size;i++)
		{
			UTF82CHAR(root["sublist"][i]["scid"].asString().c_str(),data.cid);  
			UTF82CHAR(root["sublist"][i]["sname"].asString().c_str(),data.name);  
			UTF82CHAR(root["sublist"][i]["sext"].asString().c_str(),data.ext);  
			UTF82CHAR(root["sublist"][i]["language"].asString().c_str(),data.language);  
			UTF82CHAR(root["sublist"][i]["surl"].asString().c_str(),data.url);  
			subList.push_back(data);
		}
	}  

	sd.DestroyData();

	return 1;

}