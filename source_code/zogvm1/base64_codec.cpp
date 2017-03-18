/**
 * COPYRIGHT NOTICE
 * Copyright (c) 2012, Reconova
 * All rights reserved
 * 
 * @file		base64_codec.c
 * @brief		实现base64的编码解码功能
 * @version		1.1
 * @author
 * @date
 */
#include "stdafx.h"
#include "base64_codec.h"
#include <stdio.h>
#include <string>

/**
 * Encode data to base64 and null-terminate.
 *
 * @param out       buffer for encoded data
 * @param out_size  size in bytes of the output buffer, must be at
 *                  least OUT_ENBASE64_SIZE(in_size)
 * @param in        input data buffer
 * @param in_size   size in bytes of the 'in' buffer
 * @return          number of bytes written, zero for null data or error happen
 */
int base64_encode(char *out, int out_size, const unsigned char *in, int in_size)
{
    static const char enbase64tab[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char c1, c2, c3;        
    int nDiv, nMod;    
    int i;

    /* 输入参数判断 */
    if ((out == NULL) || (in == NULL) || (out_size < OUT_ENBASE64_SIZE(in_size))) {
        return 0;
    }
     
    /* 每次取3个字节，编码成4个字符 */
    for (i = 0, nDiv = (in_size / 3); i < nDiv; i++) {
        c1 = *in++;
        c2 = *in++;
        c3 = *in++;
 
        *out++ = enbase64tab[c1 >> 2];
        *out++ = enbase64tab[((c1 << 4) | (c2 >> 4)) & 0x3f];
        *out++ = enbase64tab[((c2 << 2) | (c3 >> 6)) & 0x3f];
        *out++ = enbase64tab[c3 & 0x3f];
    }
 
    /* 编码余下的字节 */
    nMod = in_size % 3;
    if (nMod == 1) {
        c1 = *in++;
        *out++ = enbase64tab[(c1 & 0xfc) >> 2];
        *out++ = enbase64tab[(c1 & 0x03) << 4];
        *out++ = '=';
        *out++ = '=';
    } else if (nMod == 2) {
        c1 = *in++;
        c2 = *in++;
        *out++ = enbase64tab[(c1 & 0xfc) >> 2];
        *out++ = enbase64tab[((c1 & 0x03) << 4) | ((c2 & 0xf0) >> 4)];
        *out++ = enbase64tab[(c2 & 0x0f) << 2];
        *out++ = '=';
    }
 
    /* 输出加个结束符 */
    *out = '\0';
 
    return OUT_ENBASE64_SIZE(in_size);    
}

/**
 * Decode a base64-encoded string.
 *
 * @param out       buffer for decoded data
 * @param out_size  size in bytes of the out buffer, must be at
 *                  least 3/4 of the length of in
 * @param in        null-terminated input string
 * @return          number of bytes written, or zero for null data or some error happen
 */
int base64_decode(unsigned char *out, int out_size, const char *in)
{
    static const char debase64tab[256] = {
        0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,  0,  0,  0,  0,  0, 
        0,  0,  0,
        62,                                 /* '+' */
        0,  0,  0,
        63,                                 /* '/' */
        52, 53, 54, 55, 56, 57, 58, 59,     /* '0' - '9' */
        60, 61, 
        0,  0,  0,  0,  0,  0,  0,        
        0,  1,  2,  3,  4,  5,  6,  7,      /* 'A' - 'Z' */
        8,  9,  10, 11, 12, 13, 14, 15, 
        16, 17, 18, 19, 20, 21, 22, 23, 
        24, 25, 
        0,  0,  0,  0,  0,  0,
        26, 27, 28, 29, 30, 31, 32, 33,     /* 'a' - 'z' */
        34, 35, 36, 37, 38, 39, 40, 41, 
        42, 43, 44, 45, 46, 47, 48, 49, 
        50, 51,
    };
    int     i, in_size, nvalue, outbyte;

    /* 输入参数判断 */
    in_size = strlen(in);
    if ((out == NULL) || (in == NULL) || (out_size < OUT_DEBASE64_SIZE(in_size))) {
        return 0;
    }
 
    /* 取4个字符，解码到一个长整数，再经过移位得到3个字节 */
    i = outbyte = 0;
    while (i < in_size) {
        nvalue = debase64tab[*in++] << 18;
        nvalue += debase64tab[*in++] << 12;
        *out++ = (nvalue & 0x00ff0000) >> 16;
        outbyte++;
        if (*in != '=') {
            nvalue += debase64tab[*in++] << 6;
            *out++ = (nvalue & 0x0000ff00) >> 8;
            outbyte++;
            if (*in != '=') {
                nvalue += debase64tab[*in++];
                *out++ =nvalue & 0x000000ff;
                outbyte++;
            }
        }

        i += 4;
    }

    return outbyte;
}

