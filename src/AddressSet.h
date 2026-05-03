#pragma once

#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>

#include "log.h"

namespace AddressSetRuntime
{
    enum class GameBuild
    {
        Unknown,
        English,
        Japanese
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

    inline const AddressSet& GetEnglishAddressSet()
    {
        static const AddressSet value =
        {
            0x145E5EE70ull, // GetCurrentMissionCode
            0x145E62540ull, // GetIconFtexPath
            0x146acc650ull, // ShlnSetFovaImpl
        };

        return value;
    }

    inline const AddressSet& GetJapaneseAddressSet()
    {
        static const AddressSet value =
        {
            0x0ull,         // GetCurrentMissionCode
            0x147A6BD40ull, // GetIconFtexPath
            0x0ull, // ShlnSetFovaImpl
        };

        return value;
    }

    inline std::wstring GetModuleDirectory(HMODULE hModule)
    {
        wchar_t path[MAX_PATH] = {};
        if (!GetModuleFileNameW(hModule, path, MAX_PATH))
            return L"";

        std::wstring fullPath(path);
        const size_t slash = fullPath.find_last_of(L"\\/");
        if (slash == std::wstring::npos)
            return L"";

        return fullPath.substr(0, slash);
    }

    inline std::string ReadWholeFileUtf8OrAnsi(const std::wstring& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            return {};

        return std::string(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
    }

    inline std::string ToLowerAscii(std::string text)
    {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        return text;
    }

    inline GameBuild DetectGameBuildFromVersionInfo(HMODULE hGame)
    {
        const std::wstring dir = GetModuleDirectory(hGame ? hGame : GetModuleHandleW(nullptr));
        if (dir.empty())
            return GameBuild::Unknown;

        const std::wstring versionInfoPath = dir + L"\\version_info.txt";
        std::string text = ReadWholeFileUtf8OrAnsi(versionInfoPath);
        if (text.empty())
        {
            Log("[AddressSet] Failed to read version_info.txt, defaulting to English.\n");
            return GameBuild::English;
        }

        text = ToLowerAscii(text);
        Log("[AddressSet] version_info.txt = %s\n", text.c_str());

        if (text.find("mst_en") != std::string::npos)
            return GameBuild::English;

        if (text.find("mst_jp") != std::string::npos)
            return GameBuild::Japanese;

        return GameBuild::English;
    }

    inline const char* GetGameBuildName(GameBuild build)
    {
        switch (build)
        {
        case GameBuild::English:
            return "English";
        case GameBuild::Japanese:
            return "Japanese";
        default:
            return "Unknown";
        }
    }

    inline bool ResolveAddressSet(HMODULE hGame)
    {
        if (!hGame)
            return false;

        GetGameBuild() = DetectGameBuildFromVersionInfo(hGame);

        switch (GetGameBuild())
        {
        case GameBuild::English:
            GetAddressSet() = GetEnglishAddressSet();
            Log("[AddressSet] Selected English address set.\n");
            return true;

        case GameBuild::Japanese:
            GetAddressSet() = GetJapaneseAddressSet();
            Log("[AddressSet] Selected Japanese address set.\n");
            return true;

        default:
            GetAddressSet() = GetEnglishAddressSet();
            Log("[AddressSet] Unknown build, defaulting to English address set.\n");
            return true;
        }
    }
}

#define gGameBuild (::AddressSetRuntime::GetGameBuild())
#define gAddr (::AddressSetRuntime::GetAddressSet())
#define ResolveAddressSet (::AddressSetRuntime::ResolveAddressSet)
#define GetGameBuildName (::AddressSetRuntime::GetGameBuildName)
