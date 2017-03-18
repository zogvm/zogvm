#ifndef _USN_H_
#define _USN_H_


// http://hi.baidu.com/chenxiong01/item/0302e8a3b5651ade5bf1912b
// http://univasity.iteye.com/category/127688
// http://univasity.iteye.com/blog/860847
// http://univasity.iteye.com/blog/805235
// http://univasity.iteye.com/blog/805234
// https://msdn.microsoft.com/en-us/library/aa364993(VS.85).aspx
// http://download.microsoft.com/download/0/6/7/0678184e-905e-4783-9511-d4dca1f492b4/changejournal2.exe
// http://blog.csdn.net/xexiyong/article/details/17200827
// https://www.microsoft.com/msj/1099/journal2/journal2.aspx  (本文遍历文件夹是超这个的)

#include <winioctl.h>

#include "SqliteOperate.h"
#include "video.h"

#include <vector>
#include <list>
using namespace::std;
#if 0

public enum UsnJournalReturnCode
{
	INVALID_HANDLE_VALUE = -1,
	USN_JOURNAL_SUCCESS = 0,
	ERROR_INVALID_FUNCTION = 1,
	ERROR_FILE_NOT_FOUND = 2,
	ERROR_PATH_NOT_FOUND = 3,
	ERROR_TOO_MANY_OPEN_FILES = 4,
	ERROR_ACCESS_DENIED = 5,
	ERROR_INVALID_HANDLE = 6,
	ERROR_INVALID_DATA = 13,
	ERROR_HANDLE_EOF = 38,
	ERROR_NOT_SUPPORTED = 50,
	ERROR_INVALID_PARAMETER = 87,
	ERROR_JOURNAL_DELETE_IN_PROGRESS = 1178,
	USN_JOURNAL_NOT_ACTIVE = 1179,
	ERROR_JOURNAL_ENTRY_DELETED = 1181,
	ERROR_INVALID_USER_BUFFER = 1784,
	USN_JOURNAL_INVALID = 17001,
	VOLUME_NOT_NTFS = 17003,
	INVALID_FILE_REFERENCE_NUMBER = 17004,
	USN_JOURNAL_ERROR = 17005
}

public enum UsnReasonCode
{
	USN_REASON_DATA_OVERWRITE = 0x00000001,
	USN_REASON_DATA_EXTEND = 0x00000002,
	USN_REASON_DATA_TRUNCATION = 0x00000004,
	USN_REASON_NAMED_DATA_OVERWRITE = 0x00000010,
	USN_REASON_NAMED_DATA_EXTEND = 0x00000020,
	USN_REASON_NAMED_DATA_TRUNCATION = 0x00000040,
	USN_REASON_FILE_CREATE = 0x00000100,
	USN_REASON_FILE_DELETE = 0x00000200,
	USN_REASON_EA_CHANGE = 0x00000400,
	USN_REASON_SECURITY_CHANGE = 0x00000800,
	USN_REASON_RENAME_OLD_NAME = 0x00001000,
	USN_REASON_RENAME_NEW_NAME = 0x00002000,
	USN_REASON_INDEXABLE_CHANGE = 0x00004000,
	USN_REASON_BASIC_INFO_CHANGE = 0x00008000,
	USN_REASON_HARD_LINK_CHANGE = 0x00010000,
	USN_REASON_COMPRESSION_CHANGE = 0x00020000,
	USN_REASON_ENCRYPTION_CHANGE = 0x00040000,
	USN_REASON_OBJECT_ID_CHANGE = 0x00080000,
	USN_REASON_REPARSE_POINT_CHANGE = 0x00100000,
	USN_REASON_STREAM_CHANGE = 0x00200000,
	USN_REASON_CLOSE = -1
}

#endif


#define ROOT_FileReferenceNumber 1407374883553285

struct USNDirTree
{
	DWORDLONG FileReferenceNumber;  
	DWORDLONG ParentFileReferenceNumber;
	//DWORD FileAttributes;
	char path[512];
	//判断是否遍历完 用于递归
	bool flag;

};
struct USNFileTree
{
	DWORDLONG FileReferenceNumber;  
	DWORDLONG ParentFileReferenceNumber;
	//DWORD FileAttributes;
	char path[512];//完整的绝对路径。含文件名
	char name[512];//文件名
	//类型 后缀名
	int maintype;
	char fileext[8];
};

bool IsNtfs(char *str);
bool GetVolumesHandle(char c,HANDLE *hVol);
bool OpenUSNJournal(HANDLE hVol) ;
bool QueryUSNJournal(HANDLE hVol,USN_JOURNAL_DATA *UsnInfo);

bool QueryUSNJournalVolume(HANDLE hVol, NTFS_VOLUME_DATA_BUFFER *ntfsVolData);

void USNFileTreeAdd( DWORDLONG FileReferenceNumber,
					DWORDLONG ParentFileReferenceNumber,
					DWORD FileAttributes,
					char *name,
					unsigned long long *filetreetotal,
					unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree);

void USNBuildDirTree(DWORDLONG ParentFileReferenceNumber,
					 char *name,
					 unsigned long long dirtreetotal,
struct USNDirTree *dirtree);

void USNBuildFileTree(DWORDLONG RootFileReferenceNumber,
					  char *Rootname,
					  unsigned long long filetreetotal,
struct USNFileTree *filetree,
	unsigned long long dirtreetotal,
struct USNDirTree *dirtree);

void GetUSNJournalTest(HANDLE hVol,USN_JOURNAL_DATA UsnInfo,NTFS_VOLUME_DATA_BUFFER ntfsVolData,
					   unsigned long long *filetreetotal,
					   unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree);

void USNBuildFullPathTest(char *Rootname,char *NtfsStr);

bool ColseUSNJournal(HANDLE hVol,USN_JOURNAL_DATA UsnInfo);
//////////////////////////////////////////////////////////////////////////

void OUT_USNFileTreeAdd( DWORDLONG FileReferenceNumber,
						DWORDLONG ParentFileReferenceNumber,
						char *name,
						unsigned long long *filetreetotal,
						unsigned long long *filetreemax,
struct USNFileTree **filetree,int maintype,
	char *fileext);

void OUT_USNDirTreeAdd( DWORDLONG FileReferenceNumber,
					   DWORDLONG ParentFileReferenceNumber,
					   char *name,
					   unsigned long long *dirtreetotal,
					   unsigned long long *dirtreemax,
struct USNDirTree **dirtree);

void OUT_GetUSNJournal(HANDLE hVol,USN_JOURNAL_DATA UsnInfo,
					   unsigned long long *filetreetotal,
					   unsigned long long *filetreemax,
struct USNFileTree **filetree,
	unsigned long long *dirtreetotal,
	unsigned long long *dirtreemax,
struct USNDirTree **dirtree,
	list<struct FILETYPE_ST> typeList);

bool OUT_FindAllFile_USN(long long hdd_nid,char * hdd_area,
						 list<struct FILETYPE_ST> typeList,list<struct ZIDIAN_ST> zidianList);

#endif