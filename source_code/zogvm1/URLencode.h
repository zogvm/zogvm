#ifndef __URL_ENCODE_DECODE_H__
#define __URL_ENCODE_DECODE_H__

#include "stdio.h"


BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);
BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);

void EncodeURI(const char *Str,char *Dst,unsigned int DstSize);

#endif
