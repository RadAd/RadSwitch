#include "WinError.h"

#include <strsafe.h>

#include "MemoryPlus.h"
#include "Format.h"

thread_local HRESULT g_raderrorhr = ERROR_SUCCESS;

std::string WinError::getMessage(DWORD dwError, LPCSTR szModule, LPCSTR szContext)
{
    const HMODULE hLibrary = szModule != nullptr ? GetModuleHandleA(szModule) : NULL;
    auto pMessage = AutoUniquePtr<CHAR[]>(nullptr, LocalFree);
    //std::unique_ptr<CHAR[]> pMessage(nullptr, LocalFree);
    if (FormatMessageA((hLibrary == NULL ? FORMAT_MESSAGE_FROM_SYSTEM : FORMAT_MESSAGE_FROM_HMODULE) |
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        hLibrary,
        dwError,
        0,
        (LPSTR) OutPtr(pMessage).get(),
        0,
        NULL) == 0)
        return Format("Format message failed with 0x%08x", GetLastError());

    size_t len = 0;
    if (FAILED(StringCchLengthA(pMessage.get(), STRSAFE_MAX_CCH, &len)))
        return Format("StringCchLength failed with 0x%08x", GetLastError());

    pMessage[len - 2] = TEXT('\0');
    return szContext ? Format("%s -> 0x%08x %s", szContext, dwError, pMessage.get()) : Format("0x%08x %s", dwError, pMessage.get());
}

std::wstring WinError::getMessage(DWORD dwError, LPCWSTR szModule, LPCWSTR szContext)
{
    const HMODULE hLibrary = szModule != nullptr ? GetModuleHandleW(szModule) : NULL;
    auto pMessage = AutoUniquePtr<WCHAR[]>(nullptr, LocalFree);
    //std::unique_ptr<WCHAR[]> pMessage(nullptr, LocalFree);
    if (FormatMessageW((hLibrary == NULL ? FORMAT_MESSAGE_FROM_SYSTEM : FORMAT_MESSAGE_FROM_HMODULE) |
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        hLibrary,
        dwError,
        0,
        (LPWSTR) OutPtr(pMessage).get(),
        0,
        NULL) == 0)
        return Format(L"Format message failed with 0x%08x", GetLastError());

    size_t len = 0;
    if (FAILED(StringCchLengthW(pMessage.get(), STRSAFE_MAX_CCH, &len)))
        return Format(L"StringCchLength failed with 0x%08x", GetLastError());

    pMessage[len - 2] = TEXT('\0');
    return Format(L"%s -> 0x%08x %s", szContext, dwError, pMessage.get());
}

std::error_category& win32_category() noexcept
{
    static win32_error_category w32ec;
    return w32ec;
}
