#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HHOOK keyboardHook;
BOOL uzbekMode = TRUE;
BOOL f10Down = FALSE;

// Tugmani kerakli belgiga almashtirish
wchar_t getUzbekChar(DWORD key, BOOL shift, BOOL altGr)
{
    switch (key)
    {
        case 'W':
            return shift ? L'\u015E' : L'\u015F'; // Ş ş

        case 'C':
            return shift ? L'\u00C7' : L'\u00E7'; // Ç ç

        case 'O':
            if (altGr)
                return shift ? L'\u00D6' : L'\u00F6'; // Ö ö
            return 0; // Oddiy O

        case 'G':
            if (altGr)
                return shift ? L'\u011E' : L'\u011F'; // Ğ ğ
            return 0; // Oddiy G

        default:
            return 0;
    }
}

// Unicode belgini faol oynaga yozish
void sendChar(wchar_t ch)
{
    INPUT input[2] = { 0 };

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = ch;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    input[1] = input[0];
    input[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));
}

// Klaviatura tugmalarini kuzatish
LRESULT CALLBACK keyboardProc(
    int code, WPARAM msg, LPARAM data)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)data;
        DWORD vk = key->vkCode;

        BOOL down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
        BOOL up = (msg == WM_KEYUP || msg == WM_SYSKEYUP);

        // Dastur yuborgan belgilarni qayta ishlamaslik
        if (key->flags & LLKHF_INJECTED)
            return CallNextHookEx(keyboardHook, code, msg, data);

        BOOL ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        BOOL shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        BOOL alt = (GetAsyncKeyState(VK_MENU) & 0x8000);

        // Ctrl + Shift + F10 - dasturdan chiqish
        if (vk == VK_F10 && down && ctrl && shift)
        {
            PostQuitMessage(0);
            return 1;
        }

        // F10 - Uzbek / English rejimini almashtirish
        if (vk == VK_F10)
        {
            if (down && !f10Down)
                uzbekMode = !uzbekMode;

            f10Down = down;
            return 1;
        }

        // Uzbek rejimida maxsus harflar
        if (uzbekMode && down)
        {
            BOOL altGr = alt && ctrl;

            wchar_t ch = getUzbekChar(vk, shift, altGr);

            if (ch != 0)
            {
                sendChar(ch);
                return 1;
            }
        }
    }

    return CallNextHookEx(keyboardHook, code, msg, data);
}

// Windows dasturining boshlanishi
int WINAPI WinMain(
    HINSTANCE hInst, HINSTANCE hPrev,
    LPSTR cmd, int show)
{
    keyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        keyboardProc,
        GetModuleHandleW(NULL),
        0
    );

    if (!keyboardHook)
    {
        MessageBoxW(
            NULL,
            L"Keyboard hook o'rnatilmadi!",
            L"Xatolik",
            MB_ICONERROR
        );
        return 1;
    }

    MessageBoxW(
        NULL,
        L"Uzbek Keyboard ishga tushdi!\n\n"
        L"W - ş, Shift+W - Ş\n"
        L"C - ç, Shift+C - Ç\n"
        L"AltGr+O - ö, Shift+AltGr+O - Ö\n"
        L"AltGr+G - ğ, Shift+AltGr+G - Ğ\n\n"
        L"F10 - Uzbek / English\n"
        L"Ctrl+Shift+F10 - Chiqish",
        L"Uzbek Keyboard",
        MB_OK | MB_ICONINFORMATION
    );

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}