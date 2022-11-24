#include "stdafx.h"
#include <winscard.h>
#include "BonTuner.h"
#include "../usbdevfile.h"
#include <stdlib.h>

static SCARDCONTEXT context = '9175';
static SCARDHANDLE card_handle = '9175';
static const char readerA[] = "IT9175\0";
static const WCHAR readerW[] = L"IT9175\0";

extern "C" __declspec(dllexport) const SCARD_IO_REQUEST it9175_g_rgSCardT0Pci = { SCARD_PROTOCOL_T0, 8 };
extern "C" __declspec(dllexport) const SCARD_IO_REQUEST it9175_g_rgSCardT1Pci = { SCARD_PROTOCOL_T1, 8 };
extern "C" __declspec(dllexport) const SCARD_IO_REQUEST it9175_g_rgSCardRawPci = { SCARD_PROTOCOL_RAW, 8 };

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardEstablishContext(
    _In_  DWORD dwScope,
    _Reserved_  LPCVOID pvReserved1,
    _Reserved_  LPCVOID pvReserved2,
    _Out_ LPSCARDCONTEXT phContext)
{
    if (dwScope != SCARD_SCOPE_USER && dwScope != SCARD_SCOPE_TERMINAL && dwScope != SCARD_SCOPE_SYSTEM)
    {
        *phContext = 0;
        return SCARD_E_INVALID_PARAMETER;
    }
    *phContext = context;
    if (!CreateBonDriver()->OpenTuner())
    {
        return SCARD_E_UNEXPECTED;
    }
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardTransmit(
    _In_        SCARDHANDLE hCard,
    _In_        LPCSCARD_IO_REQUEST pioSendPci,
    _In_reads_bytes_(cbSendLength) LPCBYTE pbSendBuffer,
    _In_        DWORD cbSendLength,
    _Inout_opt_ LPSCARD_IO_REQUEST pioRecvPci,
    _Out_writes_bytes_(*pcbRecvLength) LPBYTE pbRecvBuffer,
    _Inout_     LPDWORD pcbRecvLength)
{
    BYTE buf[256];
    int rlen;
    if (hCard != card_handle)
        return SCARD_E_INVALID_HANDLE;
    if (pioSendPci->cbPciLength != 8 && pioSendPci->dwProtocol != SCARD_PROTOCOL_T1)
        return SCARD_E_INVALID_PARAMETER;
    if (((CBonTuner*)CreateBonDriver())->TransmitCard((LPBYTE)pbSendBuffer, cbSendLength, buf, sizeof(buf), &rlen))
    {
        return SCARD_E_UNEXPECTED;
    }
    if (rlen > 4)
    {
        // remove NAD, PCB, len, EDC
        memcpy(pbRecvBuffer, buf + 3, rlen - 4);
        *pcbRecvLength = rlen - 4;
    }
    else
    {
        *pcbRecvLength = 0;
    }
    return SCARD_S_SUCCESS;
}

_Success_(return == SCARD_S_SUCCESS)
extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardListReadersA(
    _In_     SCARDCONTEXT hContext,
    _In_opt_ LPCSTR mszGroups,
    _When_(_Old_(*pcchReaders) == SCARD_AUTOALLOCATE, _At_((LPSTR*)mszReaders, _Outptr_result_buffer_maybenull_(*pcchReaders) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcchReaders) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaders) _Post_ _NullNull_terminated_)
    LPSTR mszReaders,
    _Inout_  LPDWORD pcchReaders)
{
    LPSTR readers = mszReaders;
    if (hContext != 0 && hContext != context)
    {
        return SCARD_E_INVALID_HANDLE;
    }
    if (mszReaders == nullptr)
    {
        *pcchReaders = ARRAYSIZE(readerA);
    }
    else
    {
        if (*pcchReaders == SCARD_AUTOALLOCATE)
        {
            readers = (LPSTR)malloc(sizeof(readerA));
            if (!readers)
                return SCARD_E_NO_MEMORY;
            *(LPSTR*)mszReaders = readers;
            *pcchReaders = ARRAYSIZE(readerA);
        }
        if (*pcchReaders >= ARRAYSIZE(readerA))
        {
            memcpy(readers, readerA, *pcchReaders * sizeof(*mszReaders));
            *pcchReaders = ARRAYSIZE(readerA);
        }
        else
        {
            memcpy(readers, readerA, *pcchReaders * sizeof(*mszReaders));
            if (*pcchReaders >= 1)
                readers[*pcchReaders - 1] = 0;
            if (*pcchReaders >= 2)
                readers[*pcchReaders - 2] = 0;
        }
    }
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardFreeMemory(
    _In_ SCARDCONTEXT hContext,
    _In_ LPCVOID pvMem)
{
    free((void*)pvMem);
    return SCARD_S_SUCCESS;
}

_Success_(return == SCARD_S_SUCCESS)
extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardListReadersW(
    _In_     SCARDCONTEXT hContext,
    _In_opt_ LPCWSTR mszGroups,
    _When_(_Old_(*pcchReaders) == SCARD_AUTOALLOCATE, _At_((LPWSTR*)mszReaders, _Outptr_result_buffer_maybenull_(*pcchReaders) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcchReaders) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaders) _Post_ _NullNull_terminated_)
    LPWSTR mszReaders,
    _Inout_  LPDWORD pcchReaders)
{
    LPWSTR readers = mszReaders;
    if (hContext != 0 && hContext != context)
    {
        return SCARD_E_INVALID_HANDLE;
    }
    if (mszReaders == nullptr)
    {
        *pcchReaders = ARRAYSIZE(readerW);
    }
    else
    {
        if (*pcchReaders == SCARD_AUTOALLOCATE)
        {
            readers = (LPWSTR)malloc(sizeof(readerW));
            if (!readers)
                return SCARD_E_NO_MEMORY;
            *(LPWSTR*)mszReaders = readers;
            *pcchReaders = ARRAYSIZE(readerW);
        }
        if (*pcchReaders >= ARRAYSIZE(readerW))
        {
            memcpy(readers, readerW, *pcchReaders * sizeof(*mszReaders));
            *pcchReaders = ARRAYSIZE(readerW);
        }
        else
        {
            memcpy(readers, readerW, *pcchReaders * sizeof(*mszReaders));
            if (*pcchReaders >= 1)
                readers[*pcchReaders - 1] = 0;
            if (*pcchReaders >= 2)
                readers[*pcchReaders - 2] = 0;
        }
    }
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardReleaseContext(
    _In_      SCARDCONTEXT hContext)
{
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardConnectA(
    _In_    SCARDCONTEXT hContext,
    _In_    LPCSTR szReader,
    _In_    DWORD dwShareMode,
    _In_    DWORD dwPreferredProtocols,
    _Out_   LPSCARDHANDLE phCard,
    _Out_   LPDWORD pdwActiveProtocol)
{
    if (hContext != 0 && hContext != context)
    {
        return SCARD_E_INVALID_HANDLE;
    }
    if (strcmp(szReader, readerA))
    {
        return SCARD_E_UNKNOWN_READER;
    }
    *phCard = card_handle;
    *pdwActiveProtocol = SCARD_PROTOCOL_T1;
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardConnectW(
    _In_    SCARDCONTEXT hContext,
    _In_    LPCWSTR szReader,
    _In_    DWORD dwShareMode,
    _In_    DWORD dwPreferredProtocols,
    _Out_   LPSCARDHANDLE phCard,
    _Out_   LPDWORD pdwActiveProtocol)
{
    if (hContext != 0 && hContext != context)
    {
        return SCARD_E_INVALID_HANDLE;
    }
    if (wcscmp(szReader, readerW))
    {
        return SCARD_E_UNKNOWN_READER;
    }
    *phCard = card_handle;
    *pdwActiveProtocol = SCARD_PROTOCOL_T1;
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardDisconnect(
    _In_    SCARDHANDLE hCard,
    _In_    DWORD dwDisposition)
{
    if (hCard != card_handle)
        return SCARD_E_INVALID_HANDLE;
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardIsValidContext(
    _In_      SCARDCONTEXT hContext)
{
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardStatusA(
    _In_        SCARDHANDLE hCard,
    _When_(_Old_(*pcchReaderLen) == SCARD_AUTOALLOCATE, _At_((LPSTR*)mszReaderNames, _Outptr_result_buffer_maybenull_(*pcchReaderLen) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcchReaderLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaderLen) _Post_ _NullNull_terminated_)
    LPSTR mszReaderNames,
    _Inout_opt_ LPDWORD pcchReaderLen,
    _Out_opt_   LPDWORD pdwState,
    _Out_opt_   LPDWORD pdwProtocol,
    _When_(_Old_(*pcbAtrLen) == SCARD_AUTOALLOCATE, _At_((LPBYTE*)pbAtr, _Outptr_result_buffer_maybenull_(*pcbAtrLen) _At_(*_Curr_, _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcbAtrLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcbAtrLen) _Post_ _NullNull_terminated_)
    LPBYTE pbAtr,
    _Inout_opt_ LPDWORD pcbAtrLen)
{
    if (pcchReaderLen)
        it9175_SCardListReadersA(context, nullptr, mszReaderNames, pcchReaderLen);
    if (pdwState)
        *pdwState = SCARD_SPECIFIC;
    if (pdwProtocol)
        *pdwProtocol = SCARD_PROTOCOL_T1;
    if (pcbAtrLen)
    {
        *pcbAtrLen = 0;
        if (*pcbAtrLen == SCARD_AUTOALLOCATE)
        {
            *(LPBYTE*)pbAtr = nullptr;
        }
    }
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardStatusW(
    _In_        SCARDHANDLE hCard,
    _When_(_Old_(*pcchReaderLen) == SCARD_AUTOALLOCATE, _At_((LPWSTR*)mszReaderNames, _Outptr_result_buffer_maybenull_(*pcchReaderLen) _At_(*_Curr_, _Post_z_ _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcchReaderLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcchReaderLen) _Post_ _NullNull_terminated_)
    LPWSTR mszReaderNames,
    _Inout_opt_ LPDWORD pcchReaderLen,
    _Out_opt_   LPDWORD pdwState,
    _Out_opt_   LPDWORD pdwProtocol,
    _When_(_Old_(*pcbAtrLen) == SCARD_AUTOALLOCATE, _At_((LPBYTE*)pbAtr, _Outptr_result_buffer_maybenull_(*pcbAtrLen) _At_(*_Curr_, _Post_ _NullNull_terminated_)))
    _When_(_Old_(*pcbAtrLen) != SCARD_AUTOALLOCATE, _Out_writes_opt_(*pcbAtrLen) _Post_ _NullNull_terminated_)
    LPBYTE pbAtr,
    _Inout_opt_ LPDWORD pcbAtrLen)
{
    if (pcchReaderLen)
        it9175_SCardListReadersW(context, nullptr, mszReaderNames, pcchReaderLen);
    if (pdwState)
        *pdwState = SCARD_SPECIFIC;
    if (pdwProtocol)
        *pdwProtocol = SCARD_PROTOCOL_T1;
    if (pcbAtrLen)
    {
        *pcbAtrLen = 0;
        if (*pcbAtrLen == SCARD_AUTOALLOCATE)
        {
            *(LPBYTE*)pbAtr = nullptr;
        }
    }
    return SCARD_S_SUCCESS;
}
extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardGetStatusChangeA(
    _In_    SCARDCONTEXT hContext,
    _In_    DWORD dwTimeout,
    _Inout_ LPSCARD_READERSTATEA rgReaderStates,
    _In_    DWORD cReaders)
{
    memset(rgReaderStates, 0, sizeof(*rgReaderStates));
    rgReaderStates->dwEventState = SCARD_STATE_PRESENT;
    return SCARD_S_SUCCESS;
}

extern "C" __declspec(dllexport)
WINSCARDAPI LONG WINAPI
it9175_SCardGetStatusChangeW(
    _In_    SCARDCONTEXT hContext,
    _In_    DWORD dwTimeout,
    _Inout_ LPSCARD_READERSTATEW rgReaderStates,
    _In_    DWORD cReaders)
{
    memset(rgReaderStates, 0, sizeof(*rgReaderStates));
    rgReaderStates->dwEventState = SCARD_STATE_PRESENT;
    return SCARD_S_SUCCESS;
}
