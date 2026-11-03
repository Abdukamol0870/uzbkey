# Uzbek Keyboard

A lightweight Windows keyboard helper for typing Uzbek-specific characters.

## Features

- Type `ş` with `W` and `ç` with `C`
- Type `ö` with `AltGr + O`
- Type `ğ` with `AltGr + G`
- Use `Shift` for uppercase characters
- Press `F10` to switch between Uzbek and English modes
- Press `Ctrl + Shift + F10` to exit

## Build

Requires GCC (MinGW-w64):

```bash
gcc uzbkey.c -o uzbkey.exe -luser32 -mwindows
