#pragma once

#include <Windows.h>
#include <cstdint>

namespace AddressSetRuntime
{
    enum class GameBuild
    {
        Unknown,
        En_1_0_15_3,   // day1820
        Jp_1_0_15_3,   // day1820
        En_1_0_15_4,   // day3800
        Jp_1_0_15_4    // day3800
    };

    struct AddressSet
    {
        uintptr_t GetCurrentMissionCode = 0;
        uintptr_t GetIconFtexPath = 0;
        uintptr_t ShlnSetFovaImpl = 0;
    };

    inline GameBuild& GetGameBuild()
    {
        static GameBuild value = GameBuild::Unknown;
        return value;
    }

    inline AddressSet& GetAddressSet()
    {
        static AddressSet value{};
        return value;
    }

    const AddressSet& Get_mst_en_day3800_AddressSet();   // 1.0.15.4 english
    const AddressSet& Get_mst_jp_day3800_AddressSet();   // 1.0.15.4 japanese
    const AddressSet& Get_mst_en_day1820_AddressSet();   // 1.0.15.3 english
    const AddressSet& Get_mst_jp_day1820_AddressSet();   // 1.0.15.3 japanese
    GameBuild DetectGameBuildFromVersionInfo(HMODULE hGame);
    bool ResolveAddressSet(HMODULE hGame);
    void InstallCrashHandler();

    inline const char* GetGameBuildName(GameBuild build)
    {
        switch (build)
        {
        case GameBuild::En_1_0_15_3: return "EN 1.0.15.3";
        case GameBuild::Jp_1_0_15_3: return "JP 1.0.15.3";
        case GameBuild::En_1_0_15_4: return "EN 1.0.15.4";
        case GameBuild::Jp_1_0_15_4: return "JP 1.0.15.4";
        default:                     return "Unknown";
        }
    }

    inline bool IsEnglishBuild(GameBuild b)  { return b == GameBuild::En_1_0_15_3 || b == GameBuild::En_1_0_15_4; }
    inline bool IsJapaneseBuild(GameBuild b) { return b == GameBuild::Jp_1_0_15_3 || b == GameBuild::Jp_1_0_15_4; }
}

#define gGameBuild (::AddressSetRuntime::GetGameBuild())
#define gAddr (::AddressSetRuntime::GetAddressSet())
#define ResolveAddressSet (::AddressSetRuntime::ResolveAddressSet)
#define InstallCrashHandler (::AddressSetRuntime::InstallCrashHandler)
#define GetGameBuildName (::AddressSetRuntime::GetGameBuildName)
