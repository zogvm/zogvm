#pragma once
#ifdef WIN_PLATFORM
#include <windows.h>
#endif
#include <string>
using namespace std;

//1.  ANSI to Unicode
std::wstring ANSIToUnicode( const string& str );

//2.  Unicode to ANSI
std::string UnicodeToANSI( const wstring& str );

//3.  UTF-8 to Unicode
std::wstring UTF8ToUnicode( const string& str );
void UTF8ToUnicode( const string& str ,wstring& res );

//4.  Unicode to UTF-8    
std::string UnicodeToUTF8( const wstring& str );

std::string ws2s( const std::wstring& ws );

std::wstring s2ws( const std::string& s );

#ifndef WIN_PLATFORM
char* strupr(char* strIn);
#endif

