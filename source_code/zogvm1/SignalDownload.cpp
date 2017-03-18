#include "stdafx.h"
#include "SignalDownload.h"

#define SIGNALDOWN_DEBUG 0

#define LOWSPEEDLIMIT 32

//扩展定义
#define SD_curl_easy_setopt_EXT(A,B,C)			\
	res=curl_easy_setopt(A,B,C);				\
	if(CURLE_OK != res)  /* we failed */		\
	{											\
	fprintf(stderr, "ERROR:%s,%d,curl told us %s\n", __FILE__,__LINE__,curl_easy_strerror(res));	\
	fclose(outfile);						\
	outfile=NULL;							\
	curl_easy_cleanup(A);				\
	A=NULL;								\
	return false;							\
	}

//扩展定义
#define SD_curl_easy_setopt_EXT_NOFILE(A,B,C)	\
	res=curl_easy_setopt(A,B,C);				\
	if(CURLE_OK != res)  /* we failed */		\
	{											\
	fprintf(stderr, "ERROR:%s,%d,curl told us %s\n", __FILE__,__LINE__,curl_easy_strerror(res));	\
	curl_easy_cleanup(A);					\
	A=NULL;									\
	return false;							\
	}

//扩展定义
#define SD_curl_easy_getinfo_EXT_NOFILE(A,B,C)	\
	res=curl_easy_getinfo(A,B,C);				\
	if(CURLE_OK != res)  /* we failed */		\
	{											\
	fprintf(stderr, "ERROR:%s,%d,curl told us %s\n", __FILE__,__LINE__,curl_easy_strerror(res));	\
	curl_easy_cleanup(A);					\
	A=NULL;									\
	return false;							\
	}

//下载并写到文件中 回调函数
/*
typedef size_t (*curl_write_callback)(char *buffer,
size_t size,
size_t nitems,
void *outstream);
*/

size_t SD_WriteFunc_FILE(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

size_t SD_WriteFunc_STR(void *ptr, size_t size, size_t nmemb, void *stream)
{
	SignalDownload *p=(SignalDownload *)stream;

	int res_size;
	res_size = size * nmemb;

	if(0==p->totalstrlen)
		p->outstr = (char*) calloc(1,res_size + 1);
	else
		p->outstr =(char*) realloc(p->outstr, p->totalstrlen+res_size + 1);

	memcpy(p->outstr + p->totalstrlen, ptr, res_size);
	p->totalstrlen += res_size;

	return res_size;

}


//实时下载或上传进程的回调函数
/*
typedef int (*curl_progress_callback)(void *clientp,
double dltotal,
double dlnow,
double ultotal,
double ulnow);
*/
int SD_ProgressFunc(void *clientp,
					double dltotal, 
					double dlnow, 
					double ultotal,
					double ulnow)
{
	SignalDownload *s=(SignalDownload *)clientp;
	s->completesize=dlnow;
	s->totalsize=dltotal;

	fprintf(stderr,"%g / %g (%g %%)\r\n", dlnow, dltotal, dlnow*100.0/dltotal);

	//注意这里返回 0为正常进程
	//返回 1为退出下载进程。直接下载失败(可以做“取消下载”使用)
	return 	0;
}

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
								  TCHAR *failstr,long timeout=10,unsigned  long startsize=0,
								  bool usa=false)
{
	completesize=0;
	totalsize=0;
	//初始化
	curl = curl_easy_init();
	if(NULL == curl)
		return false;

	if(outflag)
	{
		//打开待写文件
		outfile = fopen(filename, "wb");
		if(NULL== outfile)
		{
			curl_easy_cleanup(curl);
			curl=NULL;
			return false;
		}
	}
	else
	{
		totalstrlen=0;
		if(NULL!=outstr)
		{
			free(outstr);
			outstr=NULL;
		}
	}
	//设置URL地址
	SD_curl_easy_setopt_EXT(curl, CURLOPT_URL, url);
	/*
	//设置起始下载位置
	sprintf(sizestr,"%I64u-",startsize);
	fseek(outfile,startsize,SEEK_SET);
	SD_curl_easy_setopt_EXT(curl, CURLOPT_RANGE, sizestr);
	*/
	if(proxyflag)
	{
		SD_curl_easy_setopt_EXT(curl, CURLOPT_PROXY, proxystr);
	}

	if(usa)
		curl_easy_setopt(curl,CURLOPT_USERAGENT,
		"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31");
	if(outflag)
	{	
		//设置写入的文件指针
		SD_curl_easy_setopt_EXT(curl, CURLOPT_WRITEDATA, outfile);
		//设置写入回调函数
		SD_curl_easy_setopt_EXT(curl, CURLOPT_WRITEFUNCTION, SD_WriteFunc_FILE);
	}
	else
	{
		SD_curl_easy_setopt_EXT(curl, CURLOPT_WRITEDATA, this);
		//设置写入回调函数
		SD_curl_easy_setopt_EXT(curl, CURLOPT_WRITEFUNCTION, SD_WriteFunc_STR);
	}

	SD_curl_easy_setopt_EXT(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
	//SD_curl_easy_setopt_EXT(curl, CURLOPT_SSL_VERIFYHOST, FALSE);

	//设置无进程函数
	//SD_curl_easy_setopt_EXT(curl, CURLOPT_NOPROGRESS, FALSE);
	SD_curl_easy_setopt_EXT(curl, CURLOPT_NOPROGRESS, TRUE);
	//设置进程回调函数
	//SD_curl_easy_setopt_EXT(curl, CURLOPT_PROGRESSFUNCTION, SD_ProgressFunc);
	//设置进程回调函数传的自定义参
	//SD_curl_easy_setopt_EXT(curl, CURLOPT_PROGRESSDATA, this);
	//设置下载速度=0时 N次退出
	//	SD_curl_easy_setopt_EXT(curl, CURLOPT_TIMEOUT, timeout);绝对不能出现。不然只连接timeout秒
	//设置URL地址错误 重连N次后推出
	SD_curl_easy_setopt_EXT(curl, CURLOPT_CONNECTTIMEOUT, timeout);
	//设置最低速度。为了中途拔网线
	SD_curl_easy_setopt_EXT(curl, CURLOPT_LOW_SPEED_LIMIT, LOWSPEEDLIMIT);
	SD_curl_easy_setopt_EXT(curl, CURLOPT_LOW_SPEED_TIME, timeout);

	if(SIGNALDOWN_DEBUG)
		SD_curl_easy_setopt_EXT(curl, CURLOPT_VERBOSE , 1);
	//开始执行
	res = curl_easy_perform(curl);
	if(CURLE_OK != res)  /* we failed */				
	{										
		sprintf(failstr,"HTTPDownload %s",curl_easy_strerror(res));

		fprintf(stderr, "curl result %s\n",curl_easy_strerror(res));	
		if(outflag)
		{
			//关闭文件
			fclose(outfile);
			outfile=NULL;
		}
		curl_easy_cleanup(curl);
		curl=NULL;
		return false;							
	}	

	if(outflag)
	{
		//关闭文件
		fclose(outfile);
		outfile=NULL;
	}

	curl_easy_cleanup(curl);
	curl=NULL;


	return true;
}

void SignalDownload::InitData()
{
	curl_global_init(CURL_GLOBAL_ALL);
	totalstrlen=0;
	outstr=NULL;
}

void SignalDownload::DestroyData()
{
	curl_global_cleanup();

	if(NULL!=outstr)
	{
		free(outstr);
		outstr=NULL;
	}
}