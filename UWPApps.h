#pragma once

#include <Windows.h>
#include <string>

std::wstring GetAppId(HWND hWnd);
HICON GetIconForAppId(const std::wstring& strAppId, SIZE sz);
