#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HHOOK hook;
BOOL uzbekMode = TRUE;
BOOL f10Down = FALSE;

wchar_t getUzbekChar(DWORD key, BOOL shift, BOOL altGr)
{
    switch (key)
    {
        case 'W': return shift ? L'Ş' : L'ş';
        case 'C': return shift ? L'Ç' : L'ç';
        case 'O': return altGr ? (shift ? L'Ö' : L'ö') : 0;
        case 'G': return altGr ? (shift ? L'Ğ' : L'ğ') : 0;
        default:  return 0;
    }
}

void sendChar(wchar_t ch)
{
    INPUT in[2] = {0};

    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wScan = ch;
    in[0].ki.dwFlags = KEYEVENTF_UNICODE;

    in[1] = in[0];
    in[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(2, in, sizeof(INPUT));
}

LRESULT CALLBACK keyboardProc(int code, WPARAM msg, LPARAM data)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)data;
        DWORD vk = key->vkCode;

        BOOL down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        BOOL ctrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
        BOOL shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
        BOOL alt = GetAsyncKeyState(VK_MENU) & 0x8000;

        if (key->flags & LLKHF_INJECTED)
            return CallNextHookEx(hook, code, msg, data);
        if (vk == VK_F10 && down && ctrl && shift)
        {
            PostQuitMessage(0);
            return 1;
        }

        if (vk == VK_F10)
        {
            if (down && !f10Down)
                uzbekMode = !uzbekMode;

            f10Down = down;
            return 1;
        }

        // Maxsus harflarni yozish
        if (uzbekMode && down)
        {
            wchar_t ch = getUzbekChar(vk, shift, ctrl && alt);

            if (ch)
            {
                sendChar(ch);
                return 1;
            }
        }
    }

    return CallNextHookEx(hook, code, msg, data);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show)
{
    hook = SetWindowsHookExW(
        WH_KEYBOARD_LL, keyboardProc, GetModuleHandleW(NULL), 0
    );

    if (!hook)
    {
        MessageBoxW(NULL, L"Hook o'rnatilmadi!", L"Xatolik", MB_ICONERROR);
        return 1;
    }

    MessageBoxW(NULL,
        L"Uzbek Keyboard ishga tushdi!\n\n"
        L"W / Shift+W - ş / Ş\n"
        L"C / Shift+C - ç / Ç\n"
        L"AltGr+O - ö / Ö\n"
        L"AltGr+G - ğ / Ğ\n\n"
        L"F10 - Uzbek / English\n"
        L"Ctrl+Shift+F10 - Chiqish",
        L"Uzbek Keyboard", MB_OK);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
        DispatchMessageW(&msg);

    UnhookWindowsHookEx(hook);
    return 0;
}
