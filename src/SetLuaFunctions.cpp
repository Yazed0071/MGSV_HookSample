#include "pch.h"
extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

#include <Windows.h>
#include <cstdint>
#include <unordered_set>
#include "HookUtils.h"
#include "log.h"
#include "FoxHashes.h"
#include <EquipIconFtexPathHook.h>

namespace
{
    using SetLuaFunctions_t = void(__fastcall*)(lua_State* L);
    using FoxLuaRegisterLibrary_t = void(__fastcall*)(lua_State* L, const char* libName, luaL_Reg* funcs);
    using lua_tolstring_t = const char* (__fastcall*)(lua_State* L, int idx, size_t* len);
    using lua_tointeger_t = long long(__fastcall*)(lua_State* L, int idx);
    using lua_tonumber_t = lua_Number(__fastcall*)(lua_State* L, int idx);
    using lua_pushnumber_t = void(__fastcall*)(lua_State* L, lua_Number n);

    // Absolute address of tpp::ui::UiCommand::SetLuaFunctions.
    // Params: L (lua_State*)
    static constexpr uintptr_t ABS_SetLuaFunctions = 0x1408D78A0ull;

    // Absolute address of fox::LuaRegisterLibrary.
    // Params: L (lua_State*), libName (const char*), funcs (luaL_Reg*)
    static constexpr uintptr_t ABS_FoxLuaRegisterLibrary = 0x14006B6D0ull;

    // Absolute address of the game's lua_tolstring thunk.
    // Params: L (lua_State*), idx (int), len (size_t*)
    static constexpr uintptr_t ABS_lua_tolstring = 0x141A123C0ull;

    // Absolute address of the game's lua_tointeger thunk.
    // Params: L (lua_State*), idx (int)
    static constexpr uintptr_t ABS_lua_tointeger = 0x141A12390ull;

    // Absolute address of the game's lua_tonumber thunk.
    // Params: L (lua_State*), idx (int)
    static constexpr uintptr_t ABS_lua_tonumber = 0x141A12460ull;

    // Absolute address of the game's lua_pushnumber thunk.
    // Params: L (lua_State*), n (lua_Number)
    static constexpr uintptr_t ABS_lua_pushnumber = 0x141A11BC0ull;

    static SetLuaFunctions_t       g_OrigSetLuaFunctions = nullptr;
    static FoxLuaRegisterLibrary_t g_FoxLuaRegisterLibrary = nullptr;
    static lua_tolstring_t         g_lua_tolstring = nullptr;
    static lua_tointeger_t         g_lua_tointeger = nullptr;
    static lua_tonumber_t   g_lua_tonumber = nullptr;
    static lua_pushnumber_t g_lua_pushnumber = nullptr;

    static std::unordered_set<lua_State*> g_RegisteredLuaStates;
}

// Resolves the Lua/game functions used by this bridge file.
static bool ResolveLuaApi()
{
    if (!g_FoxLuaRegisterLibrary)
    {
        g_FoxLuaRegisterLibrary = reinterpret_cast<FoxLuaRegisterLibrary_t>(
            ResolveGameAddress(ABS_FoxLuaRegisterLibrary));
    }

    if (!g_lua_tolstring)
    {
        g_lua_tolstring = reinterpret_cast<lua_tolstring_t>(
            ResolveGameAddress(ABS_lua_tolstring));
    }

    if (!g_lua_tointeger)
    {
        g_lua_tointeger = reinterpret_cast<lua_tointeger_t>(
            ResolveGameAddress(ABS_lua_tointeger));
    }

    if (!g_lua_tonumber)
    {
        g_lua_tonumber = reinterpret_cast<lua_tonumber_t>(
            ResolveGameAddress(ABS_lua_tonumber));
    }

    if (!g_lua_pushnumber)
    {
        g_lua_pushnumber = reinterpret_cast<lua_pushnumber_t>(
            ResolveGameAddress(ABS_lua_pushnumber));
    }

    return g_FoxLuaRegisterLibrary &&
        g_lua_tolstring &&
        g_lua_tointeger &&
        g_lua_tonumber &&
        g_lua_pushnumber;
}

// Registers the HookSample Lua library in the given Lua state.
static bool RegisterLuaLibrary(lua_State* L, const char* libName, luaL_Reg* funcs)
{
    if (!ResolveLuaApi() || !L || !libName || !funcs)
        return false;

    g_FoxLuaRegisterLibrary(L, libName, funcs);
    Log("[HookSample] Registered library: %s (L=%p)\n", libName, L);
    return true;
}

// Returns a Lua string argument or nullptr if unavailable.
static const char* GetLuaString(lua_State* L, int idx)
{
    if (!ResolveLuaApi() || !g_lua_tolstring)
        return nullptr;

    return g_lua_tolstring(L, idx, nullptr);
}

// Returns a Lua integer argument using the game's Lua thunk.
static int GetLuaInt(lua_State* L, int idx)
{
    if (!ResolveLuaApi() || !g_lua_tointeger)
        return 0;

    return static_cast<int>(g_lua_tointeger(L, idx));
}
// Returns a Lua number argument using the game's Lua thunk.
static float GetLuaNumber(lua_State* L, int idx)
{
    if (!ResolveLuaApi() || !g_lua_tonumber)
        return 0.0f;

    return static_cast<float>(g_lua_tonumber(L, idx));
}

// Pushes a Lua number using the game's Lua thunk.
static void PushLuaNumber(lua_State* L, float value)
{
    if (!ResolveLuaApi() || !g_lua_pushnumber)
        return;

    g_lua_pushnumber(L, static_cast<lua_Number>(value));
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