#pragma once

#include <Windows.h>
#include <string>

std::wstring GetAppId(HWND hWnd);
std::wstring GetDisplayName(const std::wstring& strAppId);
HICON GetIconForAppId(const std::wstring& strAppId, SIZE sz);
