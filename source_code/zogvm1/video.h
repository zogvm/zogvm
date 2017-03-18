#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "stdio.h"
#include "tchar.h"
#include "SqliteOperate.h"

#include <vector>
#include <list>
using namespace::std;

#if OPEN_FFMEPG
void VideoInit(void);
int VideoPlay(TCHAR* filePath,long *sec,int *w,int *h)  ;

int AudioPlay(TCHAR* filePath,long *sec) ;
#endif



int VideoInfo(TCHAR* filePath,long *sec,int *w,int *h,long *bitrateKB);
int AudioInfo(TCHAR* filePath,long *sec,long *bitrateKB) ;


long long filesize(TCHAR* filePath);
void CHAR2UTF8(const char *str,char *utf8);
void UTF82CHAR(const char *utf8,char *str);

void UTF82CHAR_SQL(char *name,int argc, char **argv, char **ColName,char *str);
long UTF82LONG(char *name,int argc, char **argv, char **ColName);
long long UTF82LONGLONG(char *name,int argc, char **argv, char **ColName);

int ClearClipboradBuffer(void);
int AppendBufferToClipboard( char *cBuffer, unsigned long  nSize );
int GetBufferFromClipboard( char *cBuffer, unsigned long  nMaxSize );

CString GetFileExt(CString fileName);
void GetFileExtStr(CString fileName,char *fileext);


CString GetFilePathNoName(CString filepath);
int checkExt(list<struct FILETYPE_ST> typeList,char *fileext) ;

void BuildFirstExt(list<struct FILETYPE_ST> typeList,char *firstExt,int *maxlen) ;
bool checkFirstExt(int maxlen,char *firstExt,char *fileext) ;

void StrSplite(list<struct STR_SPLITE_S> &strList,char *src) ;
void checkStr(struct FILE_ST &data,list<struct ZIDIAN_ST> zidianList,list<struct STR_SPLITE_S> strList) ;
void FindAllFile(long long hdd_nid,CString hdd_area,
				 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList);

void FindAllFile_NEW(long long hdd_nid,CString hdd_area,
					 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList);

bool SVsubDownload_ComputeFileHash(char *dst,char *filepath);
int SVsubDownload_Post(char *out,char *md5str,char *Chn_eng);
bool SVsubDownload_Get(char *url,char *filepath) ;
int SVsubDownload_Download(char *md5str,char *path,bool onlyone,int chn);

void CheckAllDoubleFile(long long hdd_nid,char *path);

void CheckAllIsFile(long long hdd_nid,char *path);
void GetEd2kUrl(char* path,char *name,char *dst);
void HashInit(void);
void GetHASH(char* path,char *ed2k,char *aich,char *btih);

char* ConvertString(char * strText);
bool FileDeleteToUndo(char *path);


struct XUNLEI_SUB_ST
{
	char cid[64];
	char name[256]; //√˚◊÷
	char  ext[8]; //¿‡–Õ
	char  language[32]; //”Ô—‘
	char url[128];
};

int GetXunleiSub(char *name,list<struct XUNLEI_SUB_ST> &subList);



#endif