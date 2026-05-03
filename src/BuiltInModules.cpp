#include "pch.h"

#include <Windows.h>
#include <mutex>

#include "BuiltInModules.h"
#include "FeatureModule.h"

bool Install_SetLuaFunctions_Hook();
bool Uninstall_SetLuaFunctions_Hook();

bool Install_EquipIconFtexPath_Hook();
bool Uninstall_EquipIconFtexPath_Hook();

bool Install_ShlnSetFovaImpl_Hook();
bool Uninstall_ShlnSetFovaImpl_Hook();

namespace
{
    class LuaBridgeModule final : public IFeatureModule
    {
    public:
        const char* GetName() const override
        {
            return "LuaBridge";
        }

        bool Install(HMODULE hGame) override
        {
            UNREFERENCED_PARAMETER(hGame);
            return Install_SetLuaFunctions_Hook();
        }

        void Uninstall() override
        {
            Uninstall_SetLuaFunctions_Hook();
        }
    };

    class EquipIconFtexPathModule final : public IFeatureModule
    {
    public:
        const char* GetName() const override
        {
            return "EquipIconFtexPath";
        }

        bool Install(HMODULE hGame) override
        {
            UNREFERENCED_PARAMETER(hGame);
            return Install_EquipIconFtexPath_Hook();
        }

        void Uninstall() override
        {
            Uninstall_EquipIconFtexPath_Hook();
        }
    };
    class ShlnSetFovaImplModule final : public IFeatureModule
    {
    public:
        const char* GetName() const override
        {
            return "ShlnSetFovaImpl";
        }

        bool Install(HMODULE hGame) override
        {
            UNREFERENCED_PARAMETER(hGame);
            return Install_ShlnSetFovaImpl_Hook();
        }

        void Uninstall() override
        {
            Uninstall_ShlnSetFovaImpl_Hook();
        }
    };
}

void RegisterBuiltInFeatureModules()
{
    static LuaBridgeModule         s_LuaBridgeModule;
    static EquipIconFtexPathModule s_EquipIconFtexPathModule;
    static ShlnSetFovaImplModule s_ShlnSetFovaImplModule;

    static std::once_flag s_Once;
    std::call_once(s_Once, []()
        {
            FeatureModuleRegistry::Instance().Register(&s_LuaBridgeModule);
            FeatureModuleRegistry::Instance().Register(&s_EquipIconFtexPathModule);
            FeatureModuleRegistry::Instance().Register(&s_ShlnSetFovaImplModule);
        });
}
