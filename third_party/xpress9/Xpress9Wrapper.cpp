#include "Xpress9Wrapper.h"

//
// Initialize the wrapper with the specified compression level.
//
// Return value
//
//  TRUE if initialization succeeded, FALSE if not.
//
BOOL XPress9Wrapper::Initialize()
{
    BOOL result = false;

    // Initialize XPress9
    XPRESS9_STATUS status = {0};

    UINT runtimeFlags = 0;

    // Initialize XPress9 decoder
    m_decoder9 = Xpress9DecoderCreate(
                                            &status,
                                            NULL,
                                            XpressAllocMemoryCb,
                                            XPRESS9_WINDOW_SIZE_LOG2_MAX,
                                            runtimeFlags);

    if (m_decoder9 == NULL || status.m_uStatus != Xpress9Status_OK)
    {
        std::cout << "Failed to initialize XPress9 decoder" << std::endl;
        // goto exit;
        throw std::runtime_error(status.m_ErrorDescription);
    }

    Xpress9DecoderStartSession(&status, m_decoder9, true);

    if (status.m_uStatus != Xpress9Status_OK)
    {
        // goto exit;
        throw std::runtime_error(status.m_ErrorDescription);
    }
    

    result = true;

exit:

    return result;
}


//
// Frees the resources associated with the xpress operations.
//
VOID XPress9Wrapper::Terminate()
{
    XPRESS9_STATUS status = {0};

    if (m_decoder9)
    {
        Xpress9DecoderDestroy(&status, m_decoder9, NULL, XpressFreeMemoryCb);
        m_decoder9 = NULL;
    }
}


//
// Decompress the data from the specified compress buffer.
//
// Arguments
//
//  compressed - Buffer containing compressed data.
//
//  compressedSize - Number of bytes in the compressed buffer.
//
//  original - Buffer to receive the decompressed data.
//
//  maxOriginalSize - Number of bytes available in the original buffer.
//
// Return value
//
//  Returns the number of bytes in the original buffer. If this is zero the
//  decompress failed.
//
UINT XPress9Wrapper::Decompress(
    BYTE * compressed,
    INT compressedSize,
    BYTE * original,
    INT maxOriginalSize)
{
    UINT originalSize = 0;
    BOOL detach = false;
    XPRESS9_STATUS status = {0};

    // std::cout << "Decompressing " << compressedSize << " bytes" << std::endl;

    // Decompress with XPress9
    Xpress9DecoderAttach(&status, m_decoder9, compressed, compressedSize);

    if (status.m_uStatus != Xpress9Status_OK)
    {
        // goto exit;
        throw std::runtime_error(status.m_ErrorDescription);
    }

    detach = true;

    UINT bytesRemaining;

    do
    {
        UINT bytesWritten;
        UINT compressedBytesConsumed;

        bytesRemaining = Xpress9DecoderFetchDecompressedData(
                                                        &status,
                                                        m_decoder9,
                                                        original,
                                                        maxOriginalSize,
                                                        &bytesWritten,
                                                        &compressedBytesConsumed);

        if (status.m_uStatus != Xpress9Status_OK)
        {
            originalSize = 0;
            // goto exit;
            throw std::runtime_error(status.m_ErrorDescription);
        }

        if (bytesWritten == 0)
        {
            break;
        }

        originalSize += bytesWritten;
    }
    while (bytesRemaining != 0);
    

exit:

    if (detach)
    {
        Xpress9DecoderDetach (&status, m_decoder9, compressed, compressedSize);
    }

    return originalSize;

}

