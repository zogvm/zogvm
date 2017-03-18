#include "stdafx.h"
#include "StdString.h"


//1.  ANSI to Unicode
wstring ANSIToUnicode( const string& str )
{
#ifdef WIN_PLATFORM
	int len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);  
	wchar_t* pUnicode = NULL;  
	pUnicode = new wchar_t[unicodeLen+1];
    memset(pUnicode, 0, (unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);  
	wstring rt;
	rt = (wchar_t*)pUnicode;
	delete []pUnicode; 
	return rt; 
#else
	wstring rt;
	return rt;
#endif
  
}


//2.  Unicode to ANSI
string UnicodeToANSI( const wstring& str )
{
#ifdef WIN_PLATFORM
	char* pElementText;
	int iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen+1));
	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
#else
	string rt;
	return rt;
#endif 

}


//3.  UTF-8 to Unicode
wstring UTF8ToUnicode( const string& str )
{
#ifdef WIN_PLATFORM
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);  
	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
    wstring  rt;  
	rt = (wchar_t*)pUnicode;
	delete pUnicode; 
	return rt; 
#else
	wstring rt;
	return rt;
#endif 
  
}


//4.  Unicode to UTF-8    
string UnicodeToUTF8( const wstring& str )
{
#ifdef WIN_PLATFORM
	char* pElementText;
	int iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen+1];
	memset((void*)pElementText,0,sizeof(char)*(iTextLen+1));
	::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, pElementText, iTextLen, NULL, NULL);
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
#else
	string rt;
	return rt;
#endif 
  
}





std::string ws2s( const std::wstring& ws )
{
#ifdef WIN_PLATFORM
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
#else


	std::string val = "";  
	if(!ws.c_str())  
	{  
		return val;  
	}  
	//size_t size= wcslen(pw)*sizeof(wchar_t);  
	size_t size= ws.length() * sizeof(wchar_t);  
	char *pc = NULL;  
	if ( !(pc = (char*)malloc(size)) )  
	{  
		return val;  
	}  
	size_t destlen = wcstombs(pc,ws.c_str(),size);  
	/*转换不为空时，返回值为-1。如果为空，返回值0*/  
	if ( destlen == (size_t)(0) )  
	{  
		return val;  
	}  
	val = pc;  
	free(pc);  
	return val; 
#endif
 
}

std::wstring s2ws( const std::string& s )
{
#ifdef WIN_PLATFORM
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	return result;
#endif 
//    wstring rt;
//    return rt;
    std::wstring val = L"";  

    if ( NULL == s.c_str() )
    {  
        return val;  
    }  
    //size_t size_of_ch = strlen(pc)*sizeof(char);  
    //size_t size_of_wc = get_wchar_size(pc); 

    size_t size_of_wc;  
    size_t destlen = mbstowcs(0,s.c_str(),0);  
    if (destlen ==(size_t)(-1))  
    {  
        return val;  
    }  
    size_of_wc = destlen+1;  
    wchar_t *pw  = new wchar_t[size_of_wc];  
    mbstowcs(pw,s.c_str(),size_of_wc);  
    val = pw;  
    delete pw;  
    return val;  

}

#ifndef WIN_PLATFORM
char* strupr(char* strIn)
{
    char* ptr = strIn;
    while( *ptr != '\0' )
    {
        if ( islower(*ptr) ) 
        {
            *ptr = toupper(*ptr);
        }
    }
    return strIn;
}
#endif
