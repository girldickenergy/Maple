#include "ClipboardUtilities.h"

#include <windows.h>

std::string ClipboardUtilities::Read()
{
    if (!OpenClipboard(nullptr))
        return {};

    const HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        CloseClipboard();

        return {};
    }
	
    const char* data = static_cast<char*>(GlobalLock(hData));
    if (data == nullptr)
    {
        GlobalUnlock(hData);
        CloseClipboard();

        return {};
    }

    GlobalUnlock(hData);

    CloseClipboard();
	
    return data;
}

void ClipboardUtilities::Write(const std::string& data)
{
    if (!OpenClipboard(nullptr))
        return;
	
    if (!EmptyClipboard())
    {
        CloseClipboard();

        return;
    }

    const HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, data.size());
    if (!hg)
    {
        CloseClipboard();

        return;
    }

    memcpy(GlobalLock(hg), data.c_str(), data.size());

    GlobalUnlock(hg);

    SetClipboardData(CF_TEXT, hg);

    CloseClipboard();

    GlobalFree(hg);
}
