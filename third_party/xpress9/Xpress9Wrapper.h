#pragma once 
#include <stdlib.h>
#include <algorithm>
#include "xpress.h"
#include "xpress9.h"
#include <stdexcept>
#include <iostream>
// Define relevant types
using BOOL = bool;
using UINT = unsigned int;
using BYTE = unsigned char;
using INT = int;
using VOID = void;

#define UNREFERENCED_PARAMETER(P) (void)(P)

//
// Lightweight wrapper on top of XPress library. 
//
class XPress9Wrapper
{
public:

    inline static const INT c_MaxOriginalSize = std::min(64 * 1024, XPRESS_MAX_BLOCK);

public:

    XPress9Wrapper()
    {
        m_decoder9 = NULL;
    }

    ~XPress9Wrapper()
    {
        Terminate();
    }

public:

    BOOL Initialize();

    VOID Terminate();

    UINT Decompress(
        BYTE * compressed,
        INT compressedSize,
        BYTE * original,
        INT maxOriginalSize);

private:

    // Decoder
    XPRESS9_DECODER m_decoder9;

private:

    static void * XPRESS_CALL XpressAllocMemoryCb(void *Context, int AllocSize)
    {
        UNREFERENCED_PARAMETER(Context);
        return malloc(AllocSize);
    }

    static void XPRESS_CALL XpressFreeMemoryCb(void *Context, void *Address)
    {
        UNREFERENCED_PARAMETER(Context);
        free(Address);
    }
};

