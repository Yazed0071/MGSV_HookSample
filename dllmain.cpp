#include "pch.h"
#include <Windows.h>
#include <atomic>
#include <cstdio>

#include "MinHook.h"
#include "log.h"

bool Install_SetLuaFunctions_Hook();
bool Uninstall_SetLuaFunctions_Hook();

bool Install_PlayerVoiceFpk_Hook();
bool Uninstall_PlayerVoiceFpk_Hook();

namespace
{
    static std::atomic_bool gStarted{ false };
    static std::atomic_bool gConsoleReady{ false };
}

// Creates or attaches a console for debug logging.
static void SetupConsole()
{
    if (gConsoleReady.load())
        return;

    if (!AllocConsole())
        AttachConsole(ATTACH_PARENT_PROCESS);

    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    SetConsoleTitleW(L"V_FrameWork");
    gConsoleReady.store(true);

    printf("[DLL] Console ready\n");
    fflush(stdout);
}

// Initializes MinHook and all runtime hooks on a worker thread.
static DWORD WINAPI InitThread(LPVOID)
{
    #ifdef _DEBUG
    SetupConsole();
    #endif // DEBUG


    Log("[DLL] InitThread started.\n");

    const MH_STATUS st = MH_Initialize();
    Log("[DLL] MH_Initialize -> %d\n", static_cast<int>(st));
    if (st != MH_OK && st != MH_ERROR_ALREADY_INITIALIZED)
        return 0;

    const bool okLua = Install_SetLuaFunctions_Hook();
    Log("[DLL] Install_SetLuaFunctions_Hook: %s\n", okLua ? "OK" : "FAIL");

	const bool okPlayerVoiceFpk = Install_PlayerVoiceFpk_Hook();
	Log("[DLL] Install_PlayerVoiceFpk_Hook: %s\n", okPlayerVoiceFpk ? "OK" : "FAIL");

    Log("[DLL] InitThread done.\n");
    return 0;
}

// Removes all hooks when the DLL unloads normally.
static void UninstallAll(bool processTerminating)
{
    if (processTerminating)
        return;

    Uninstall_SetLuaFunctions_Hook();
	Uninstall_PlayerVoiceFpk_Hook();

    MH_Uninitialize();
    Log("[DLL] UninstallAll done.\n");

    fflush(stdout);
    fflush(stderr);
}

// Standard Windows DLL entry point.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        bool expected = false;
        if (!gStarted.compare_exchange_strong(expected, true))
            return TRUE;

        HANDLE hThread = CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);

        return TRUE;
    }

    case DLL_PROCESS_DETACH:
    {
        UninstallAll(lpReserved != nullptr);
        return TRUE;
    }
    }

    return TRUE;
}