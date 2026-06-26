#include "pch.h"

#include <Windows.h>
#include <cstdint>
#include <unordered_set>

#include "LuaApi.h"
#include "HookUtils.h"
#include "log.h"
#include <EquipIconFtexPathHook.h>

namespace
{
    using SetLuaFunctions_t = void(__fastcall*)(lua_State* L);

    // Absolute address of tpp::ui::UiCommand::SetLuaFunctions.
    // Params: L (lua_State*)
    static constexpr uintptr_t ABS_SetLuaFunctions = 0x1408D78A0ull;

    static SetLuaFunctions_t g_OrigSetLuaFunctions = nullptr;

    static std::unordered_set<lua_State*> g_RegisteredLuaStates;
}

// Sets one equip icon override.
// Params: equipId, path
static int __cdecl l_SetEquipIconFtexPath(lua_State* L)
{
    const int   equipId = GetLuaInt(L, 1);
    const char* rawPath = GetLuaString(L, 2);

    if (!rawPath || !*rawPath)
        return 0;

    Set_EquipIconFtexPath(equipId, rawPath);
    return 0;
}

// Clears one equip icon override.
// Params: equipId
static int __cdecl l_ClearEquipIconFtexPath(lua_State* L)
{
    const int equipId = GetLuaInt(L, 1);
    Clear_EquipIconFtexPath(equipId);
    return 0;
}

// Clears all equip icon overrides.
// Params: none
static int __cdecl l_ClearAllEquipIconFtexPaths(lua_State* L)
{
    UNREFERENCED_PARAMETER(L);
    Clear_AllEquipIconFtexPaths();
    return 0;
}

static luaL_Reg g_HookSample[] =
{
    { "SetEquipIconFtexPath",                   l_SetEquipIconFtexPath },
    { "ClearEquipIconFtexPath",                 l_ClearEquipIconFtexPath },
    { "ClearAllEquipIconFtexPaths",             l_ClearAllEquipIconFtexPaths },

    { nullptr, nullptr }
};

// Registers HookSample into a UI Lua state only once.
static void RegisterAllUiLuaLibraries(lua_State* L)
{
    if (!L)
        return;

    if (g_RegisteredLuaStates.find(L) != g_RegisteredLuaStates.end())
        return;

    RegisterLuaLibrary(L, "HookSample", g_HookSample);
    g_RegisteredLuaStates.insert(L);
}


// Hooked version of SetLuaFunctions that appends HookSample registration.
static void __fastcall hkSetLuaFunctions(lua_State* L)
{
    g_OrigSetLuaFunctions(L);
    RegisterAllUiLuaLibraries(L);
}

// Exported Lua loader for require("HookSample").
extern "C" __declspec(dllexport) int __cdecl luaopen_HookSample(lua_State* L)
{
    return RegisterLuaLibrary(L, "HookSample", g_HookSample) ? 1 : 0;
}

// Installs the SetLuaFunctions hook.
bool Install_SetLuaFunctions_Hook()
{
    ResolveLuaApi();

    void* target = ResolveGameAddress(ABS_SetLuaFunctions);
    if (!target)
        return false;

    const bool ok = CreateAndEnableHook(
        target,
        reinterpret_cast<void*>(&hkSetLuaFunctions),
        reinterpret_cast<void**>(&g_OrigSetLuaFunctions));

    Log("[Hook] SetLuaFunctions: %s\n", ok ? "OK" : "FAIL");
    return ok;
}

// Removes the SetLuaFunctions hook.
bool Uninstall_SetLuaFunctions_Hook()
{
    DisableAndRemoveHook(ResolveGameAddress(ABS_SetLuaFunctions));
    g_OrigSetLuaFunctions = nullptr;
    g_RegisteredLuaStates.clear();
    return true;
}
