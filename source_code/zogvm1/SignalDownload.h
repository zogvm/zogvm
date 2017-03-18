#ifndef __SIGNAL_DOWNLOAD_H
#define __SIGNAL_DOWNLOAD_H
#include "typedefine.h"
#include "tchar.h"
class SignalDownload
{
public:
	//url		URL地址 
	//outflag	1文件模式 0字符串模式
	//filename	文件路径
	//proxyflag	1开启代理 0不开代理
	//failstr	失败信息
	//timeout	设定下载超时时限
	//输出		下载成功或失败
	//执行下载
	bool SignalDownload::HTTPDownload(const TCHAR *url,
		bool outflag,const TCHAR *filename,
		bool proxyflag,TCHAR *proxystr,
		TCHAR *failstr,long timeout,unsigned  long startsize,  bool usa);
	//当前完成字节
	double completesize;
	//文件总字节
	double totalsize;

	void InitData();
	void DestroyData();

	char *outstr ;
	long totalstrlen;
private:
	CURL *curl;
	CURLcode res;

	FILE *outfile;
	char sizestr[128];

};

#endif