#include "UWPApps.h"

#include <atlbase.h>
#include <propsys.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shellapi.h>
#include <ShlObj.h>

#include "Rad/Log.h"
#include "Rad/Format.h"

#include <vector>

#include "UWPResources.h"

std::wstring GetPropertyStoreString(IPropertyStore* pStore, REFPROPERTYKEY key)
{
    PROPVARIANT val;
    PropVariantInit(&val);
    std::wstring res;
    if (SUCCEEDED(pStore->GetValue(key, &val)))
    {
        switch (val.vt)
        {
        case VT_EMPTY:
            break;

        case VT_LPWSTR: case VT_BSTR:
            res = val.pwszVal;
            break;

#if 0 // TODO Convert
        case VT_LPSTR:
            res = val.pszVal;
            break;
#endif

        default:
            ATLASSERT(FALSE);
            break;
        }
    }
    PropVariantClear(&val);
    return res;
}

#if 0
void DumpPropertyStore(IPropertyStore* pStore)
{
    DWORD count = 0;
    CHECK_HR(pStore->GetCount(&count));
    for (DWORD i = 0; i < count; ++i)
    {
        PROPERTYKEY key = {};
        CHECK_HR(pStore->GetAt(i, &key));

        TCHAR strkey[1024];
        CHECK_HR(PSStringFromPropertyKey(key, strkey, ARRAYSIZE(strkey)));

        PROPVARIANT val = {};
        PropVariantInit(&val);
        CHECK_HR(pStore->GetValue(key, &val));

        TCHAR strval[1024];
        PropVariantToString(val, strval, ARRAYSIZE(strval));

        OutputDebugString(Format(TEXT("%s = %s\n"), strkey, strval).c_str());

        CHECK_HR(PropVariantClear(&val));
    }
}
#endif

std::vector<BYTE> LoadFile(LPCTSTR strFileName)
{
    const HANDLE hTextFile = CreateFile(strFileName, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hTextFile == INVALID_HANDLE_VALUE)
        return {};
    const DWORD dwFileSize = GetFileSize(hTextFile, nullptr);

    std::vector<BYTE> data(dwFileSize);

    DWORD dwBytesRead;
    CHECK_LE(ReadFile(hTextFile, data.data(), dwFileSize, &dwBytesRead, NULL));
    _ASSERTE(dwBytesRead == dwFileSize);

    CloseHandle(hTextFile);

    return data;
}

std::wstring GetAppId(HWND hWnd)
{
    CComPtr<IPropertyStore> pPropStore;
    CHECK_HR(SHGetPropertyStoreForWindow(hWnd, IID_IPropertyStore, (void**) &pPropStore));
    if (!pPropStore)
        return {};

    //DumpPropertyStore(pPropStore);

    return GetPropertyStoreString(pPropStore, PKEY_AppUserModel_ID);
}

std::wstring GetDisplayName(const std::wstring& strAppId)
{
    CComPtr<IShellItem2> target;
    CHECK_HR_RET(SHCreateItemInKnownFolder(FOLDERID_AppsFolder, 0, strAppId.c_str(), IID_PPV_ARGS(&target)), NULL);

    CComPtr<IPropertyStore> pStore;
    CHECK_HR_RET(target->BindToHandler(NULL, BHID_PropertyStore, IID_PPV_ARGS(&pStore)), NULL);

    //DumpPropertyStore(pStore);

    return GetPropertyStoreString(pStore, PKEY_ItemNameDisplay);
}

HICON GetIconForAppId(const std::wstring& strAppId, SIZE sz)
{
    CComPtr<IShellItem2> target;
    CHECK_HR_RET(SHCreateItemInKnownFolder(FOLDERID_AppsFolder, 0, strAppId.c_str(), IID_PPV_ARGS(&target)), NULL);

    CComPtr<IPropertyStore> pStore;
    CHECK_HR_RET(target->BindToHandler(NULL, BHID_PropertyStore, IID_PPV_ARGS(&pStore)), NULL);

    //DumpPropertyStore(pStore);

    const std::wstring strPackageName = GetPropertyStoreString(pStore, PKEY_MetroPackageName);
    if (strPackageName.empty())
        return NULL;

    const std::wstring strLogo = GetPropertyStoreString(pStore, PKEY_Tile_Small);
    if (strLogo.empty())
        return NULL;

    CComPtr<IResourceManager> pResManager;
    CHECK_HR_RET(pResManager.CoCreateInstance(CLSID_ResourceManager), NULL);
    CHECK_HR_RET(pResManager->InitializeForPackage(strPackageName.c_str()), NULL);

    CComPtr<IResourceContext> pResContext;
    CHECK_HR_RET(pResManager->GetDefaultContext(IID_PPV_ARGS(&pResContext)), NULL);

    CHECK_HR_RET(pResContext->SetTargetSize(WORD(sz.cx)), NULL);
    CHECK_HR_RET(pResContext->SetScale(RES_SCALE_80), NULL);
    CHECK_HR_RET(pResContext->SetContrast(RES_CONTRAST_STANDARD), NULL);

    CComPtr<IResourceMap> pResMap;
    CHECK_HR_RET(pResManager->GetMainResourceMap(IID_PPV_ARGS(&pResMap)), NULL);

    CComHeapPtr<WCHAR> pFilePath;
    CHECK_HR_RET(pResMap->GetFilePath(strLogo.c_str(), &pFilePath), NULL);

    std::vector<BYTE> data = LoadFile(pFilePath);

    return CreateIconFromResourceEx(data.data(), DWORD(data.size()), TRUE, 0x00030000, sz.cx, sz.cy, LR_DEFAULTCOLOR);
}
