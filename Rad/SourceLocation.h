#pragma once
#include <stddef.h> // for wchar_t in C

struct SourceLocationA
{
    long line;
    const char* file;
    const char* function;
    const char* funcsig;
};

#ifdef __cplusplus
#define SrcLocA SourceLocationA
#define SRC_LOC_A SourceLocationA({ __LINE__, __FILE__, __FUNCTION__, __FUNCSIG__ })
#else
#define SrcLocA struct SourceLocationA
#define SRC_LOC_A (struct SourceLocationA) { __LINE__, __FILE__, __FUNCTION__, __FUNCSIG__ }
#endif

struct SourceLocationW
{
    long line;
    const wchar_t* file;
    const wchar_t* function;
    const wchar_t* funcsig;
};

#ifdef __cplusplus
#define SrcLocW SourceLocationW
#define SRC_LOC_W SourceLocationW({ __LINE__, __FILEW__, __FUNCTIONW__, _CRT_WIDE(__FUNCSIG__) })
#else
#define SrcLocW struct SourceLocationW
#define SRC_LOC_W (struct SourceLocationW) { __LINE__, __FILEW__, __FUNCTIONW__, _CRT_WIDE(__FUNCSIG__) }
#endif


#ifdef _UNICODE
#define SrcLoc SrcLocW
#define SRC_LOC SRC_LOC_W
#else
#define SrcLoc SrcLocA
#define SRC_LOC SRC_LOC_A
#endif
