#include "pch.h"

#include "log.h"
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

static FILE* g_LogFile = nullptr;

void InitLog()
{
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    char* lastSlash = strrchr(path, '\\');
    if (lastSlash) *(lastSlash + 1) = '\0';
    strcat_s(path, "HookSample.log");

    fopen_s(&g_LogFile, path, "w");
    if (g_LogFile)
        fprintf(g_LogFile, "[LOG] Log file created successfully.\n");
}

void EnsureConsole()
{
#ifdef _DEBUG
    if (GetConsoleWindow())
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        return;
    }

    if (!AllocConsole())
        AttachConsole(ATTACH_PARENT_PROCESS);

    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    SetConsoleTitleW(L"HookSample");
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
}

void Log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    if (g_LogFile)
    {
        va_list args2;
        va_start(args2, fmt);
        vfprintf(g_LogFile, fmt, args2);
        va_end(args2);
        fflush(g_LogFile);
    }

    va_end(args);
}

void CrashLogf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    if (g_LogFile)
    {
        va_list args2;
        va_start(args2, fmt);
        vfprintf(g_LogFile, fmt, args2);
        va_end(args2);
        fflush(g_LogFile);
    }
}

void CloseLog()
{
    if (g_LogFile)
    {
        fprintf(g_LogFile, "[LOG] Closing log.\n");
        fclose(g_LogFile);
        g_LogFile = nullptr;
    }
}
