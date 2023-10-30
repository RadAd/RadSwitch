#pragma once

#include <combaseapi.h>
#include <propkey.h>

///////////////////////////////////////////////////////////////////////////////

// See https://learn.microsoft.com/en-us/previous-versions/windows/apps/hh965372(v=win.10)#scale
enum RESOURCE_SCALE
{
    RES_SCALE_100 = 0,
    RES_SCALE_140 = 1,
    RES_SCALE_180 = 2,
    RES_SCALE_80 = 3,
};

// See https://learn.microsoft.com/en-us/previous-versions/windows/apps/hh965372(v=win.10)#contrast
enum RESOURCE_CONTRAST
{
    RES_CONTRAST_STANDARD = 0,
    RES_CONTRAST_HIGH = 1,
    RES_CONTRAST_BLACK = 2,
    RES_CONTRAST_WHITE = 3,
};

DECLARE_INTERFACE_IID_(IResourceContext, IUnknown, "E3C22B30-8502-4B2F-9133-559674587E51")
{
    STDMETHOD(GetLanguage)(LPWSTR * pLanguage) PURE;
    STDMETHOD(GetHomeRegion)(LPWSTR * pRegion) PURE;
    STDMETHOD(GetLayoutDirection)(enum RESOURCE_LAYOUT_DIRECTION* pDirection) PURE;
    STDMETHOD(GetTargetSize)(WORD * pSize) PURE;
    STDMETHOD(GetScale)(RESOURCE_SCALE * pScale) PURE;
    STDMETHOD(GetContrast)(RESOURCE_CONTRAST * pContrast) PURE;
    STDMETHOD(GetAlternateForm)(LPWSTR * pForm) PURE;
    STDMETHOD(GetQualifierValue)(LPCWSTR name, LPWSTR * pValue) PURE;
    STDMETHOD(SetLanguage)(LPCWSTR language) PURE;
    STDMETHOD(SetHomeRegion)(LPCWSTR region) PURE;
    STDMETHOD(SetLayoutDirection)(enum RESOURCE_LAYOUT_DIRECTION direction) PURE;
    STDMETHOD(SetTargetSize)(WORD size) PURE;
    STDMETHOD(SetScale)(RESOURCE_SCALE scale) PURE;
    STDMETHOD(SetContrast)(RESOURCE_CONTRAST contrast) PURE;
    STDMETHOD(SetAlternateForm)(LPCWSTR form) PURE;
};

///////////////////////////////////////////////////////////////////////////////

DECLARE_INTERFACE_IID_(IResourceMap, IUnknown, "6E21E72B-B9B0-42AE-A686-983CF784EDCD")
{
    STDMETHOD(GetUri)(LPCWSTR * pUri) PURE;
    STDMETHOD(GetSubtree)(LPCWSTR propName, IResourceMap * *pSubTree) PURE;
    STDMETHOD(GetString)(LPCWSTR propName, LPWSTR * pString) PURE;
    STDMETHOD(GetStringForContext)(IResourceContext * pContext, LPCWSTR propName, LPWSTR * pString) PURE;
    STDMETHOD(GetFilePath)(LPCWSTR propName, LPWSTR * pPath) PURE;
    STDMETHOD(GetFilePathForContext)(IResourceContext * pContext, LPCWSTR propName, LPWSTR * pPath) PURE;
};

///////////////////////////////////////////////////////////////////////////////

DEFINE_GUID(CLSID_ResourceManager, 0xDBCE7E40, 0x7345, 0x439D, 0xB1, 0x2C, 0x11, 0x4A, 0x11, 0x81, 0x9A, 0x09);

// IMrtResourceManager
DECLARE_INTERFACE_IID_(IResourceManager, IUnknown, "130A2F65-2BE7-4309-9A58-A9052FF2B61C")
{
public:
    STDMETHOD(Initialize)(void) PURE;
    STDMETHOD(InitializeForCurrentApplication)(void) PURE;
    STDMETHOD(InitializeForPackage)(LPCWSTR name) PURE;
    STDMETHOD(InitializeForFile)(LPCWSTR fname) PURE;
    STDMETHOD(GetMainResourceMap)(REFIID riid, void** ppvObject) PURE;
    STDMETHOD(GetResourceMap)(LPCWSTR name, REFIID riid, void** ppvObject) PURE;
    STDMETHOD(GetDefaultContext)(REFIID riid, void** ppvObject) PURE;
    STDMETHOD(GetReference)(REFIID riid, void** ppvObject) PURE;
    STDMETHOD(Proc11)(LPCWSTR p0, UINT * p1) PURE;
};

DEFINE_PROPERTYKEY(PKEY_Tile_SmallLogoPath, 0x86D40B4D, 0x9069, 0x443C, 0x81, 0x9A, 0x2A, 0x54, 0x09, 0x0D, 0xCC, 0xEC, 2);
DEFINE_PROPERTYKEY(PKEY_Tile_Small, 0x86D40B4D, 0x9069, 0x443C, 0x81, 0x9A, 0x2A, 0x54, 0x09, 0x0D, 0xCC, 0xEC, 12);
DEFINE_PROPERTYKEY(PKEY_Small_Tile, 0x86D40B4D, 0x9069, 0x443C, 0x81, 0x9A, 0x2A, 0x54, 0x09, 0x0D, 0xCC, 0xEC, 20);
DEFINE_PROPERTYKEY(PKEY_MetroPackageName, 0x9F4C2855, 0x9F79, 0x4B39, 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3, 21);
