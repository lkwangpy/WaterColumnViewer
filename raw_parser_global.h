#ifndef XTF_PARSER_GLOBAL_H
#define XTF_PARSER_GLOBAL_H

#define XTF_PARSER_VERSION       0x000001
#define XTF_PARSER_VERSION_STR   "0.0.1"

#ifdef XTF_PARSER_LIBRARY
#define XTF_PARSER_DLL_API __declspec(dllexport)
#else
#define XTF_PARSER_DLL_API __declspec(dllimport)
#endif

typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

//! 7k
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;
typedef unsigned long long	u64;

typedef double d64;

//! em
typedef char s8;
typedef short s16;
typedef int s32;
//#pragma comment(lib, "Ws2_32.lib")

//! r2sonic use 7k & em

#include <vector>
#include <string>
#include <map>

#endif // XTF_PARSER_GLOBAL_H
