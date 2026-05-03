#include "pch.h"

#include <Windows.h>
#include <cstdint>
#include <mutex>
#include <unordered_map>

#include "AddressSet.h"
#include "EquipIconFtexPathHook.h"
#include "FoxHashes.h"
#include "HookUtils.h"
#include "MissionCodeGuard.h"
#include "log.h"

namespace
{
    // Absolute address of tpp::ui::utility::GetIconFtexPath.
    // Params: outPathId (int64_t*), equipId (uint32_t), mode (int)
    using GetIconFtexPath_t = std::int64_t* (__fastcall*)(std::int64_t* outPathId, std::uint32_t equipId, int mode);

    static GetIconFtexPath_t g_OrigGetIconFtexPath = nullptr;

    static std::unordered_map<int, std::uint64_t> g_PerEquipIconPaths;
    static std::mutex                             g_PerEquipIconPathsMutex;
    static bool                                   g_HookInstalled = false;
}

// Hooked entry: returns the override path when present, otherwise defers to the game.
// Params: outPathId (int64_t*), equipId (uint32_t), mode (int)
static std::int64_t* __fastcall hkGetIconFtexPath(std::int64_t* outPathId, std::uint32_t equipId, int mode)
{
    if (MissionCodeGuard::ShouldBypassHooks())
    {
        return g_OrigGetIconFtexPath(outPathId, equipId, mode);
    }

    if (outPathId)
    {
        std::lock_guard<std::mutex> lock(g_PerEquipIconPathsMutex);

        const auto it = g_PerEquipIconPaths.find(static_cast<int>(equipId));
        if (it != g_PerEquipIconPaths.end())
        {
            *outPathId = static_cast<std::int64_t>(it->second);

            Log("[EquipIcon] Override hit: equipId=%u mode=%d -> 0x%016llX\n",
                equipId,
                mode,
                static_cast<unsigned long long>(it->second));

            return outPathId;
        }
    }

    return g_OrigGetIconFtexPath(outPathId, equipId, mode);
}

// Installs the equip icon path selector hook.
// Params: none
bool Install_EquipIconFtexPath_Hook()
{
    if (g_HookInstalled)
    {
        Log("[Hook] EquipIconFtexPath: already installed\n");
        return true;
    }

    void* target = ResolveGameAddress(gAddr.GetIconFtexPath);
    if (!target)
    {
        Log("[Hook] EquipIconFtexPath: target resolve failed\n");
        return false;
    }

    const bool ok = CreateAndEnableHook(
        target,
        reinterpret_cast<void*>(&hkGetIconFtexPath),
        reinterpret_cast<void**>(&g_OrigGetIconFtexPath)
    );

    g_HookInstalled = ok;
    Log("[Hook] EquipIconFtexPath: %s\n", ok ? "OK" : "FAIL");
    return ok;
}

// Removes the equip icon path selector hook and clears stored overrides.
// Params: none
bool Uninstall_EquipIconFtexPath_Hook()
{
    if (!g_HookInstalled)
        return true;

    DisableAndRemoveHook(ResolveGameAddress(gAddr.GetIconFtexPath));
    g_OrigGetIconFtexPath = nullptr;
    g_HookInstalled = false;

    {
        std::lock_guard<std::mutex> lock(g_PerEquipIconPathsMutex);
        g_PerEquipIconPaths.clear();
    }

    Log("[Hook] EquipIconFtexPath: removed\n");
    return true;
}

// Sets a per-equipId override from a raw .ftex path string.
// Params: equipId (int), rawPath (const char*)
void Set_EquipIconFtexPath(int equipId, const char* rawPath)
{
    if (!rawPath || !*rawPath)
        return;

    const std::uint64_t pathCode64Ext = FoxHashes::PathCode64Ext(rawPath);

    {
        std::lock_guard<std::mutex> lock(g_PerEquipIconPathsMutex);
        g_PerEquipIconPaths[equipId] = pathCode64Ext;
    }

    Log("[EquipIcon] Override set: equipId=%d path=%s -> 0x%016llX\n",
        equipId,
        rawPath,
        static_cast<unsigned long long>(pathCode64Ext));
}

// Clears the override for one equipId.
// Params: equipId (int)
void Clear_EquipIconFtexPath(int equipId)
{
    {
        std::lock_guard<std::mutex> lock(g_PerEquipIconPathsMutex);
        g_PerEquipIconPaths.erase(equipId);
    }

    Log("[EquipIcon] Override cleared: equipId=%d\n", equipId);
}

// Clears every registered override.
// Params: none
void Clear_AllEquipIconFtexPaths()
{
    {
        std::lock_guard<std::mutex> lock(g_PerEquipIconPathsMutex);
        g_PerEquipIconPaths.clear();
    }

    Log("[EquipIcon] All overrides cleared\n");
}
