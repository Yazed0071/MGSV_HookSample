#include "pch.h"
extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
}

#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <unordered_set>
#include <mutex>

#include "HookUtils.h"
#include "log.h"

#include "AddressSet.h"
#include "LuaApi.h"
#include "EquipIconFtexPathHook.h"


namespace
{
    using SetLuaFunctions_t = void(__fastcall*)(lua_State* L);

    static SetLuaFunctions_t       g_OrigSetLuaFunctions = nullptr;

    static std::unordered_set<lua_State*> g_RegisteredLuaStates;
    static std::mutex g_RegisteredLuaStatesMutex;
    static bool g_SetLuaFunctionsHookInstalled = false;
}


static bool IsLuaStateRegistered(lua_State* L)
{
    std::lock_guard<std::mutex> lock(g_RegisteredLuaStatesMutex);
    return g_RegisteredLuaStates.find(L) != g_RegisteredLuaStates.end();
}


static void TrackLuaState(lua_State* L)
{
    std::lock_guard<std::mutex> lock(g_RegisteredLuaStatesMutex);
    g_RegisteredLuaStates.insert(L);
}


static void ClearTrackedLuaStates()
{
    std::lock_guard<std::mutex> lock(g_RegisteredLuaStatesMutex);
    g_RegisteredLuaStates.clear();
}


static int __cdecl l_SetEquipIconFtexPath(lua_State* L)
{
    const int   equipId = GetLuaInt(L, 1);
    const char* rawPath = GetLuaString(L, 2);

    if (!rawPath || !*rawPath)
        return 0;

    Set_EquipIconFtexPath(equipId, rawPath);
    return 0;
}


static int __cdecl l_ClearEquipIconFtexPath(lua_State* L)
{
    const int equipId = GetLuaInt(L, 1);
    Clear_EquipIconFtexPath(equipId);
    return 0;
}


static int __cdecl l_ClearAllEquipIconFtexPaths(lua_State* L)
{
    UNREFERENCED_PARAMETER(L);
    Clear_AllEquipIconFtexPaths();
    return 0;
}


static luaL_Reg g_HookSampleLib[] =
{
    { "SetEquipIconFtexPath",       l_SetEquipIconFtexPath },
    { "ClearEquipIconFtexPath",     l_ClearEquipIconFtexPath },
    { "ClearAllEquipIconFtexPaths", l_ClearAllEquipIconFtexPaths },

    { nullptr, nullptr }
};


static void RegisterAllUiLuaLibraries(lua_State* L)
{
    if (!L)
        return;

    if (IsLuaStateRegistered(L))
        return;

    if (RegisterLuaLibrary(L, "HookSample", g_HookSampleLib))
    {
        TrackLuaState(L);
    }
}


static void __fastcall hkSetLuaFunctions(lua_State* L)
{
    if (g_OrigSetLuaFunctions)
    {
        g_OrigSetLuaFunctions(L);
    }

    RegisterAllUiLuaLibraries(L);
}


extern "C" __declspec(dllexport) int __cdecl luaopen_HookSample(lua_State* L)
{
    if (!L)
        return 0;

    if (IsLuaStateRegistered(L))
        return 0;

    if (!RegisterLuaLibrary(L, "HookSample", g_HookSampleLib))
        return 0;

    TrackLuaState(L);
    return 1;
}


bool Install_SetLuaFunctions_Hook()
{
    if (g_SetLuaFunctionsHookInstalled)
    {
        Log("[Hook] SetLuaFunctions: already installed\n");
        return true;
    }

    ResolveLuaApi();

    void* target = ResolveGameAddress(gAddr.SetLuaFunctions);
    if (!target)
        return false;

    const bool ok = CreateAndEnableHook(
        target,
        reinterpret_cast<void*>(&hkSetLuaFunctions),
        reinterpret_cast<void**>(&g_OrigSetLuaFunctions));

    if (ok)
    {
        g_SetLuaFunctionsHookInstalled = true;
    }

    Log("[Hook] SetLuaFunctions: %s target=%p orig=%p\n",
        ok ? "OK" : "FAIL",
        target,
        g_OrigSetLuaFunctions);
    return ok;
}


bool Uninstall_SetLuaFunctions_Hook()
{
    DisableAndRemoveHook(ResolveGameAddress(gAddr.SetLuaFunctions));
    g_OrigSetLuaFunctions = nullptr;
    ClearTrackedLuaStates();
    return true;
}
