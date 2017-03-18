#include "stdafx.h"
#include "URLencode.h"



//百分号编码
//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81
//用于报文
BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
{
    if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
        return FALSE;
	
    size_t len_ascii = strlen(szSrc);
    if(len_ascii == 0)
    {
        pBuf[0] = 0;
        return TRUE;
    }
    
    //先转换到UTF-8
    char baseChar = bUpperCase ? 'A' : 'a';
    int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
    LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
    if(pUnicode == NULL)
        return FALSE;
    MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);
	
    int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
    LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
    if(pUTF8 == NULL)
    {
        free(pUnicode);
        return FALSE;
    }
    WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
    pUTF8[cbUTF8] = '\0';
	
    unsigned char c;
    int cbDest = 0; //累加
    unsigned char *pSrc = (unsigned char*)pUTF8;
    unsigned char *pDest = (unsigned char*)pBuf;
    while(*pSrc && cbDest < cbBufLen - 1)
    {
        c = *pSrc;
        if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
        {
            (*pDest) = c;
            ++pDest;
            ++cbDest;
        }
        else if(c == ' ')
        {
            (*pDest) = '+';
            ++pDest;
            ++cbDest;
        }
        else
        {
            //检查缓冲区大小是否够用？
            if(cbDest + 3 > cbBufLen - 1)
                break;
            pDest[0] = '%';
            pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
            pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
            pDest += 3;
            cbDest += 3;
        }
        ++pSrc;
    }
    //null-terminator
    (*pDest) = '\0';
    free(pUnicode);
    free(pUTF8);
    return TRUE;
}

//解码后是utf-8编码 用于报文
BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
    if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
        return FALSE;
	
    size_t len_ascii = strlen(szSrc);
    if(len_ascii == 0)
    {
        pBuf[0] = 0;
        return TRUE;
    }
    
    char *pUTF8 = (char*)malloc(len_ascii + 1);
    if(pUTF8 == NULL)
        return FALSE;
	
    int cbDest = 0; //累加
    unsigned char *pSrc = (unsigned char*)szSrc;
    unsigned char *pDest = (unsigned char*)pUTF8;
    while(*pSrc)
    {
        if(*pSrc == '%')
        {
            (*pDest) = 0;
            //高位
            if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
                (*pDest) += (pSrc[1] - 'A' + 10) * 0x10;
            else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
                (*pDest) += (pSrc[1] - 'a' + 10) * 0x10;
            else
                (*pDest) += (pSrc[1] - '0') * 0x10;
			
            //低位
            if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
                (*pDest) += (pSrc[2] - 'A' + 10);
            else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
                (*pDest) += (pSrc[2] - 'a' + 10);
            else
                (*pDest) += (pSrc[2] - '0');
			
            pSrc += 3;
        }
        else if(*pSrc == '+')
        {
            (*pDest) = ' ';
            ++pSrc;
        }
        else
        {
            (*pDest) = *pSrc;
            ++pSrc;
        }
        ++pDest;
        ++cbDest;
    }
    //null-terminator
    (*pDest) = '\0';
    ++cbDest;
	
    int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
    LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
    if(pUnicode == NULL)
    {
        free(pUTF8);
        return FALSE;
    }
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
    WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
    free(pUTF8);
    free(pUnicode);
    return TRUE;
}


//用于浏览器地址的
void EncodeURI(const char *Str,char *Dst,unsigned int DstSize)
{
	wchar_t *Bufw = NULL;
	char *Bufc = NULL;

	unsigned long needSize = MultiByteToWideChar(CP_ACP,
		NULL,
		Str,
		-1,
		NULL,
		0);
	
	Bufw = new wchar_t[needSize];
	if ( NULL == Bufw )
	{
		printf("new申请内存出错了!");
		return ;
	}
	
	memset(Bufw,0x0,needSize*2);
	MultiByteToWideChar(CP_ACP,
		NULL,
		Str,
		-1,
		Bufw,
		needSize);
	
	needSize = WideCharToMultiByte(CP_UTF8,
		NULL,
		Bufw,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	if ( 0 == needSize )
	{
		if ( NULL != Bufw )
		{
			delete [] Bufw;
			Bufw = NULL;
		}
		if ( NULL != Bufc )
		{
			delete [] Bufc;
			Bufc = NULL;
		}
	}
	
	Bufc = new char[needSize];
	if ( NULL == Bufc )
	{
		if ( NULL != Bufw )
		{
			delete [] Bufw;
			Bufw = NULL;
		}
		if ( NULL != Bufc )
		{
			delete [] Bufc;
			Bufc = NULL;
		}
	}
	
	memset(Bufc,0x0,needSize);
	WideCharToMultiByte(CP_UTF8,
		NULL,
		Bufw,
		-1,
		Bufc,
		needSize,
		NULL,
		NULL);
	
	unsigned char *pWork = (unsigned char *)Bufc;
	memset(Dst,0x0,sizeof(Dst));
	if ( strlen(Bufc) > DstSize )
	{
		if ( NULL != Bufw )
		{
			delete [] Bufw;
			Bufw = NULL;
		}
		if ( NULL != Bufc )
		{
			delete [] Bufc;
			Bufc = NULL;
		}
	}
	while( *pWork != 0x0 )
	{
		if ( *pWork != '!' && *pWork != '@' && *pWork != '#' &&
			*pWork != '$' && *pWork != '&' && *pWork != '*' &&
			*pWork != '(' && *pWork != ')' && *pWork != '=' &&
			*pWork != ':' && *pWork != '/' && *pWork != ';' &&
			*pWork != '?' && *pWork != '+' && *pWork != '\'' &&
			*pWork != '.' && 	(*pWork <'0' ||*pWork >'9') &&
			(*pWork <'a' ||*pWork >'z') && (*pWork <'A' ||*pWork >'Z'))
		{
			sprintf(Dst+strlen(Dst),"%%%2X",*pWork);
		}
		else
		{
			sprintf(Dst+strlen(Dst),"%c",*pWork);
		}
		pWork++;
	}
	
	if ( NULL != Bufw )
	{
		delete [] Bufw;
		Bufw = NULL;
	}
	if ( NULL != Bufc )
	{
		delete [] Bufc;
		Bufc = NULL;
	}
}