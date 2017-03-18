#ifndef _CSERVICE_H_
#define _CSERVICE_H_
#include "typedefine.h"
using namespace std;
struct PostData { 
	const char *data; 
	int  datalen;
	string filename; 
}; 

struct PutData {
	const char *data;
	int datalen;
	int postion;
	int totallen;
};


typedef struct _GETPROCESS_
{
	int version;
    //去掉WINAPI 需要检查 windows 中是否正常
    void ( *GetProcessFunc )( struct _GETPROCESS_* pThis, int nProcess );
}GETPROCESS,*LPGEPROCESS;

typedef struct _FILEWRITE_
{
	
	/** 
	* @brief  Version number of the interface. Currently must be 1.
	*/
	int version;
	
	/** 
	* Output a block of data in your custom way.
	* @param[out] pThis	- Pointer to the structure itself
	* @param[out] pData	- Pointer to a buffer to output
	* @param[in]  size	- The size of the buffer.
	* @retval Should be non-zero if successful, zero for error.
	*/
	int ( *WriteBlock )( _FILEWRITE_* pThis, const void* pData, unsigned long size );
	
}FILEWRITE,*LPFILEWRITE;


typedef struct _FILEACCESS_
{
	// File length, in bytes.
	unsigned long	m_FileLen;
        
        int ( *GetFileSize )( _FILEACCESS_* pThis, unsigned long* size ); 

	int ( *GetBlock )( _FILEACCESS_* pThis, unsigned long position, unsigned char** pBuf, unsigned long size );

}FILEACCESS, *LPFILEACCESS;




class Service 
{ 
public: 
	static void InitData();
	static void DestroyData();
	Service();
	Service( const string& service_name, const string& application_name ); 
	~Service() {} 

	const string& auth_token() const { return auth_token_; } 
	void set_auth_token( const string& token ) { auth_token_ = token; } 

	const string& service_name() const { return service_name_; } 
	void set_service_name( const string &name ) { service_name_ = name; } 

	const string& application_name() const { return application_name_; } 
	void set_application_name( const string& name ) { application_name_ = name; } 

	const vector<string>& request_headers() const { return request_headers_; } 
	void set_request_headers( const vector<string>& headers ) { 
		request_headers_ = headers; 
	} 

	//入口
 	int HttpRequest( const string http_method, const string url );

 	//int HttpRequest( const string http_method, const string url, const PostData& post_data);

	int HttpRequest( const string http_method,const string url,const char *data,long datalen,string filename,int postion,int totallen);


// 	int HttpRequest( const string& http_method, const string& url, const vector<string>& custom_headers,  string& resp_buff );
// 	int HttpRequest( const string& http_method, const string& url, const PostData& post_data, string& resp_buff );
// 	int HttpRequest( const string http_method, const string url, const vector<string>& custom_headers, const PutData* lpPutData, string& resp_buffer, string& resp_header );

//	int HttpRequest( const string http_method,const string url, const vector<string>& custom_headers, const PutData* lpPutData, Service* lpService);
	//私有
	int HttpRequest( const string http_method, const string url, const vector<string>& custom_headers,  const PostData* lpPost_data,const PutData* lpPutData,Service* lpService);



public:
	string				error_str;
	wstring				m_szServerName;
	wstring				m_szUserName;
    int					m_IsConvert;
	string				m_resp_buffer;
	string				m_resp_header;
    GETPROCESS*			m_pGetProcess;
    GETPROCESS*			m_pGetDownLoadProcess;
	FILEWRITE*			m_pFileWriteStruct;
	FILEACCESS*			m_pFileAccessStruct;
	void				SetUploadProcess( GETPROCESS* pGetProcess );
	void				SetDownloaddProcess( GETPROCESS* pGetDownLoadProcess );
	void				SetFileWrite( FILEWRITE* pFileWriteStruct );
protected: 
	string				auth_token_; 
	string				service_name_; 
	string				application_name_; 
	vector<string>		request_headers_; 
	const PutData*		m_lpPutData;			

private: 
	
	static int SeekCallback( void *userp, curl_off_t offset, int whence );

	static size_t WriteCallback( char* data, size_t size, size_t nitems, void* userp ); 
	static size_t WriteHeaderCallback( char *data, size_t size, size_t nmemb, void* userp );
	static int ProgressCallback( const void* userp, double dltotal, double dlnow, double ultotal, double ulnow );
	static size_t UploadCallback( void *buffer, size_t sz, size_t nmemb, void *userp );

}; 
#endif
