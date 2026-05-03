#include "pch.h"

#include <Windows.h>
#include <cstdint>

#include "HookUtils.h"
#include "log.h"
#include "AddressSet.h"
#include "ShlnSetFovaImpl.h"


namespace {
    using ShlnSetFovaImpl_t = void(__fastcall*)(uint8_t* param_1, uint32_t param_2);
    static ShlnSetFovaImpl_t g_OrigShlnSetFovaImpl = nullptr;
}


static void __fastcall hkShlnSetFovaImpl(uint8_t* param_1, uint32_t param_2)
{
    Log("[ShlnSetFovaImpl] param_1=%p param_2=%u\n", param_1, param_2);
    g_OrigShlnSetFovaImpl(param_1, param_2);
}


bool Install_ShlnSetFovaImpl_Hook()
{
    void* target = ResolveGameAddress(gAddr.ShlnSetFovaImpl);
    if (!target)
    {
        Log("[Hook] ShlnSetFovaImpl: target resolve failed\n");
        return false;
    }

    const bool ok = CreateAndEnableHook(
        target,
        reinterpret_cast<void*>(&hkShlnSetFovaImpl),
        reinterpret_cast<void**>(&g_OrigShlnSetFovaImpl)
    );

    Log("[Hook] ShlnSetFovaImpl: %s\n", ok ? "OK" : "FAIL");
    return ok;
}


bool Uninstall_ShlnSetFovaImpl_Hook()
{
    DisableAndRemoveHook(ResolveGameAddress(gAddr.ShlnSetFovaImpl));
    g_OrigShlnSetFovaImpl = nullptr;

    Log("[Hook] ShlnSetFovaImpl: removed\n");
    return true;
}