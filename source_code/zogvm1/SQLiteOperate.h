
#ifndef _SQLITE_OPERATE_
#define _SQLITE_OPERATE_

#include "sqlite3.h"
#include <list>
using namespace::std;

#pragma comment( lib, "sqlite3.lib")



enum MAINTYPE_EM
{
	MAINTYPE_OTHER=0,
	MAINTYPE_VIDEO=1,
	MAINTYPE_MUSIC=2,
	MAINTYPE_SUB=3,
};

enum ZIDIAN_EM
{
	ZIDIAN_YAZHI=0,
	ZIDIAN_HUAZHI=1,
	ZIDIAN_FENBIANLV=2,
	ZIDIAN_3D=3,
	ZIDIAN_YEAR=4,
	ZIDIAN_YUYAN=5,

};

enum DEALSTEP_EM
{
	DEALSTEP_START=0,
	DEALSTEP_FIND_END=1,
	DEALSTEP_ALL_END=2,
};

#pragma pack(push) //保存对齐状态
#pragma pack(1)

struct STR_SPLITE_S
{
	char s[520];
};

struct ZIDIAN_ST
{
	long long  zidian_nid;
	int mainzidian; //主题类型标记
	char  zidian[16]; //类型英文
	char  lowzidian[16]; //类型英文小写
};

struct FILETYPE_ST
{
	long long  type_nid;
	int maintype; //主题类型标记
	char type[16]; //类型后缀英文
	int enable; //是否启用
};

//硬盘序列号 分区
struct HDD_ST
{
	long long  hdd_nid; 
	char serno[32];//硬盘序列号  
	char area[32];//分区 
	char mark[128]; //标签
	int insertflag; //是否接入
	int enable;	//是否扫描
	long long TotalBytes; //总空间
	long long FreeBytes; //剩余空间
	int dealstep;	//仅用来判断线程是否处理完 不进数据库
	char format[32];	//盘格式
};

//目录扫描
struct DIR_ST
{
	long long dir_nid;
	long long  hdd_nid; 
	char path[520];//目录
	int enable;	//是否扫描
};


struct FILE_ST
{
	long long  file_nid;
	char name[520];	//纯文件名
	char path[520];//不含分区的路径
	long long hdd_nid;  //硬盘ID

	int maintype; //主题类型标记
	char type[16]; //文件类型
	int resolutionW; //分辨率宽高
	int resolutionH;

	long long filesize; //文件长度
	long filetime;		//文件时间
	long bitrateKbps;	//比特率

	char md5[64];
	//文件修改时间
	long long CreationTime;
	long long LastWriteTime;

	//如果是字幕 显示他的关联视频NID
	long long video_nid;

	char zidian_3d[16]; //3D字典
	char zidian_fenbianlv[16]; //分辨率字典
	char zidian_yazhi[16]; //压制组
	char zidian_huazhi[16]; //画质
	char zidian_year[16]; //年代

	//当为1时 跳过 是否存在的检测并标志位设为0
	//当为0时 不跳过 是否存在的检测
	int newflag;	
	long long double_nid; //重复ID

	char ed2k[36]; //电驴地址
	char aich[36];   //电驴地址
	char btih[36];   //磁力地址

	char Ed2kUrl[512]; //电驴地址
	char svsubmd5[160];//射手字幕MD5
	int upflag;//上传服务器
};

struct FILE_VIEW_ST
{
	struct HDD_ST hdd_data;
	struct FILE_ST file_data;
};

//查重
struct DOUBLE_FILE_ST
{
	long long  double_nid;
	long long  file_nid;
};

//为了加速查重速度和容量
struct FILE_VIEW_LITE_ST
{
	char hdd_area[32];//分区 
	long long  file_nid;
	char file_name[520];	//纯文件名
	char file_path[520];//不含分区的路径
	int file_maintype; //主题类型标记
	long long filesize; //文件长度
	char file_md5[64];
	long long double_nid; //重复ID
};
//为了加速射手字幕
struct FILE_SVSUB_LITE_ST
{
	long long  file_nid;
	char path[520];//不含分区的路径
	char svsubmd5[160];
};

//计算HASH
struct FILE_HASH_LITE_ST
{
	long long  file_nid;
	char path[520];//不含分区的路径

	char name[520]; //仅名字
	long long filesize; //文件长度

	char ed2k[36]; //电驴地址
	char aich[36];   //电驴地址
	char btih[36];   //磁力地址
};

struct YYETS_ST
{
	long long  yyets_nid;
	char name[520];	//纯文件名
	char filesize[32]; //文件长度
	char magnet[3024];
	char ed2k[3024];
	char wanpan[128];
};


struct EXTERN_ED2K_ST
{
	long long  nid;
	char name[520];	//纯文件名
	long long filesize; //文件长度
	char ed2k[3024];
};

enum POINT_EM
{
	POINT_PATH=0,
	POINT_NAME=1,
	POINT_CREATTIME=2,
	POINT_LASTTIME=3,
	POINT_FILESIZE=4,
	POINT_FENBIANLV=5,
	POINT_INSERT=6,
	POINT_RESOLUTION=7,
	POINT_FILETIME=8,
	POINT_BITRATE=9,
};

//排序用的标记
struct UPDOWN_ST
{
	bool issearch;
	bool type_video;
	bool type_audio;
	bool type_sub;
	bool type_other;

	POINT_EM point; 

	bool path;
	bool name;
	bool creattime;
	bool lasttime;
	bool filesize;
	bool fenbianlv;
	bool insert;

	bool resolution;
	bool filetime;
	bool bitrate;

};

//字幕来源 射手
enum SUBSRC_EM
{
	SUBSRC_SV=0,
};


struct SYSTEM_SET_ST
{
	char version[8];
	int checkProxy;  
	char Proxy[256];
	int subsrc;
	int checkOnlyOne;
	int checkOnlyChn;
	//某些大小不扫描
	int c_noscan_video_min;
	long	e_noscan_video_min;
	int	c_noscan_video_max;
	long	e_noscan_video_max;
	int	c_noscan_audio_min;
	long	e_noscan_audio_min;
	int	c_noscan_audio_max;
	long	e_noscan_audio_max;
	int c_mediainfo;
	int	w_x;
	int	w_y;
	int	w_w;
	int	w_h;
};

//计算HASH
struct FILE_SOAP_ST
{
	char name[520]; //仅名字
	long long filesize; //文件长度

	char ed2k[36]; //电驴地址
	char aich[36];   //电驴地址
	char btih[36];   //磁力地址
};

//计算HASH FOR READ
struct FILE_SOAP_READ_ST
{
	long long file_nid;
	char name[520]; //仅名字
	long long filesize; //文件长度

	char ed2k[36]; //电驴地址
	char aich[36];   //电驴地址
	char btih[36];   //磁力地址
	char ed2kurl[1024];   //电驴地址
};

#pragma pack(pop)

class CSqliteOperate
{
public:
	sqlite3 * m_pDB;
	sqlite3_stmt* stmt;
public:
	CSqliteOperate(void);
	virtual ~CSqliteOperate(void);

	bool IOwriteLock;


	//打开数据库
	int OpenDB(char *file);
	int ChangeKey(char *key,int num);
	//关闭数据库
	void CloseDB();
	//执行SQL语句
	void Sql_UpdateExecute(char *sql);
	void Sql_FindExecute(char *sql);
	int Sql_CallBackExecute(char *sql,int (*callback)(void*,int,char**,char**), void *list);
	void Begin(void);
	void Commit(void);

	void filterstring(char *str,char *dst);
	void filterstringForLike(char *str,char *dst);
	void filterstringNameForLike(char *str,char *dst);

	void ZiDian_CreateTable(void);
	void ZiDian_Add(int mainzidian,char *zidian);
	bool ZiDian_Read(list<struct ZIDIAN_ST> &List);

	void Double_CreateTable(void);
	void Double_Add(long long file_nid);
	bool Double_Read(list<struct DOUBLE_FILE_ST> &List);
	void Double_ReadOne(long long *double_nid,long long file_nid);
	void Double_DeleteAll(void);
	void File_CleanDouble2Zero(void);

	void Type_CreateTable(void);
	void Type_Add(int maintype,char *type);
	void Type_ModifyEnable(char* nid,int enable);
	bool Type_Read(list<struct FILETYPE_ST> &List);
	void Type_Add_Lock(int maintype,char *type);
	void Type_DeleteAll(void);
	void Type_AddAll(void);
	void Type_AddAllunlock(void);

	void Hdd_CreateTable(void);
	void Hdd_Add(char *serno,char *area,char *mark,char *format,long long TotalBytes,long long FreeBytes);
	void Hdd_ModifyEnable(char* nid,int enable);
	bool Hdd_Read(list<struct HDD_ST> &List);
	void Hdd_SetNonsert(void);
	void Hdd_DeleteforHdd(long long hdd_nid);

	void Dir_CreateTable(void);
	void Dir_Add(long long hdd_nid,char *path);
	void Dir_ModifyEnable(char* nid,int enable);
	bool Dir_Read(list<struct DIR_ST> &List);
	void Dir_DeleteforDir(long long dir_nid);
	void Dir_DeleteforHdd(long long hdd_nid);
	void Dir_DeleteAll(void);

	void File_CreateTable(void);
	void File_CreateView(void);
	bool File_CheckDoublePos(char *name,char *path,long long hdd_nid);

	void File_Add(struct FILE_ST data);
	void File_Rename(char* name,char* file_nid );
	void File_Delete(long long file_nid);
	void File_DeleteforHdd(long long hdd_nid);

	void File_SetNoFlagOne(long long hdd_nid);
	void File_SetNoFlagAll(void);
	void File_ModifySVSubMD5(char* nid,char *svsubmd5);
	void File_ModifySVSubMD5L(long long file_nid,char *svsubmd5);

	bool File_ReadHddAllwithVideoAudio(list<struct FILE_VIEW_LITE_ST> &List,long long hdd_nid,char *path);
	bool File_ReadHddAllandOld(list<struct FILE_VIEW_LITE_ST> &List,long long hdd_nid,char *path);

	bool File_ReadHddAllforSVSub(list<struct FILE_SVSUB_LITE_ST> &List,long long hdd_nid);
	bool File_ReadDirAllforSVSub(list<struct FILE_SVSUB_LITE_ST> &List,long long hdd_nid,char *path);

	bool File_CheckDouble(int maintype,long long size,char *name,char *md5,
		list<struct FILE_VIEW_LITE_ST> &List);


	void File_SetDouble(long long double_nid,char *file_nid_str);


	long long File_NumberForHDD(long long hdd_nid);
	bool File_ReadHddPage(list<struct FILE_VIEW_ST> &List,long long hdd_nid,bool up,long long start,int num);

	void File_ResetDouble2Zero(void);

	long long File_DoubleNumber(void);
	bool File_ReadDouble(list<struct FILE_VIEW_ST> &List,long long start,int num);

	long long File_TrashNumber(void);
	bool File_ReadTrash(list<struct FILE_VIEW_ST> &List,long long start,int num);


	void File_filterUpDown(char *SQLstr,struct UPDOWN_ST updown);

	long long File_NumberForSearch(char *SQLstr,char *str);
	bool File_ReadForSearch(char *SQLstr,char *str,list<struct FILE_VIEW_ST> &List,long long start,int num);

	long long File_Number(char *SQLstr);
	bool File_Read(char *SQLstr,list<struct FILE_VIEW_ST> &List,long long start,int num);

	//YYETS
	long long YYETS_Number(void);
	long long YYETS_NumberForSearch(char *str);
	bool YYETS_Read(list<struct YYETS_ST> &List,long long start,int num);
	bool YYETS_ReadForSearch(char *str,list<struct YYETS_ST> &List,long long start,int num);

	void ExternED2K_CreateTable(void);
	void ExternED2K_Add(struct EXTERN_ED2K_ST data);
	long long ExternED2K_Number(void);
	long long ExternED2K_NumberForSearch(char *str);
	bool ExternED2K_ReadForSearch(char *str,list<struct EXTERN_ED2K_ST> &List,long long start,int num);
	bool ExternED2K_Read(int flag,list<struct EXTERN_ED2K_ST> &List,long long start,int num);

	void File_CleanIDX(long long hdd_nid);

	void SystemSet_CreateTable(void);
	void SystemSet_Modify(struct SYSTEM_SET_ST data);
	bool CSqliteOperate::SystemSet_Read(struct SYSTEM_SET_ST &data);

	bool CSqliteOperate::File_ReadforHash(list<struct FILE_HASH_LITE_ST> &List);
	void CSqliteOperate::File_writeforHash(struct FILE_HASH_LITE_ST data);


	bool CSqliteOperate::File_Readforupflag(list<struct FILE_HASH_LITE_ST> &List);
	void CSqliteOperate::File_upflagforHash(list<struct FILE_HASH_LITE_ST> &List);


	//SOAP
	long long SOAP_Number(void);
	long long SOAP_NumberForSearch(char *str);
	bool SOAP_Read(list<struct FILE_SOAP_READ_ST> &List,long long start,int num);
	bool SOAP_ReadForSearch(char *str,list<struct FILE_SOAP_READ_ST> &List,long long start,int num);


};
















#endif