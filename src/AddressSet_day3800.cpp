#include "pch.h"

#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <cstring>
#include <iterator>
#include <string>

#include "AddressSet.h"
#include "log.h"

static const char* const kAddrFieldNames[] = {
    "GetCurrentMissionCode",
    "GetIconFtexPath",
    "ShlnSetFovaImpl",
    "FoxLuaRegisterLibrary",
    "lua_getfield",
    "lua_gettop",
    "lua_isnumber",
    "lua_isstring",
    "lua_objlen",
    "lua_pushboolean",
    "lua_pushnumber",
    "lua_rawgeti",
    "lua_settop",
    "lua_toboolean",
    "lua_tointeger",
    "lua_tolstring",
    "lua_tonumber",
    "lua_type",
    "lua_pushstring",
    "lua_createtable",
    "lua_rawset",
    "lua_settable",
    "lua_pushnil",
    "lua_next",
    "lua_gettable",
    "lua_pushvalue",
    "lua_pcall",
    "lua_pushcclosure",
};
static const int kAddrFieldCount = 28;

namespace AddressSetRuntime
{
    namespace
    {
        std::wstring GetModuleDirectory(HMODULE hModule)
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

        std::string ReadWholeFileUtf8OrAnsi(const std::wstring& path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file)
                return {};

            return std::string(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
        }

        std::string ToLowerAscii(std::string text)
        {
            std::transform(
                text.begin(),
                text.end(),
                text.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            return text;
        }
    }

    const AddressSet& Get_mst_en_day3800_AddressSet()   // 1.0.15.4 english
    {
        static const AddressSet value =
        {
            0x140911210ull, // GetCurrentMissionCode
            0x140912330ull, // GetIconFtexPath
            0x140B4E0A0ull, // ShlnSetFovaImpl
            0x14006b8c0ull, // FoxLuaRegisterLibrary
            0x141A111E0ull, // lua_getfield
            0x141A112E0ull, // lua_gettop
            0x141A11410ull, // lua_isnumber
            0x141A11440ull, // lua_isstring
            0x141A11640ull, // lua_objlen
            0x141A11750ull, // lua_pushboolean
            0x141A11950ull, // lua_pushnumber
            0x141A11AE0ull, // lua_rawgeti
            0x141A11F70ull, // lua_settop
            0x141A120C0ull, // lua_toboolean
            0x141A12120ull, // lua_tointeger
            0x141A12150ull, // lua_tolstring
            0x141A121F0ull, // lua_tonumber
            0x141A12300ull, // lua_type
            0x141A11970ull, // lua_pushstring
            0x141A10E80ull, // lua_createtable
            0x141A11B20ull, // lua_rawset
            0x141A11F40ull, // lua_settable
            0x141A11930ull, // lua_pushnil
            0x141A11600ull, // lua_next
            0x141A112B0ull, // lua_gettable
            0x141A119D0ull, // lua_pushvalue
            0x141A116C0ull, // lua_pcall
            0x141A11770ull, // lua_pushcclosure
        };
        return value;
    }

    const AddressSet& Get_mst_jp_day3800_AddressSet()   // 1.0.15.4 japanese
    {
        static const AddressSet value =
        {
            0x1409110F0ull, // GetCurrentMissionCode
            0x140912210ull, // GetIconFtexPath
            0x140B4E1D0ull, // ShlnSetFovaImpl
            0x14006B920ull, // FoxLuaRegisterLibrary
            0x141A11120ull, // lua_getfield
            0x141A11220ull, // lua_gettop
            0x141A11350ull, // lua_isnumber
            0x141A11380ull, // lua_isstring
            0x141A11580ull, // lua_objlen
            0x141A11690ull, // lua_pushboolean
            0x141A11890ull, // lua_pushnumber
            0x141A11A20ull, // lua_rawgeti
            0x141A11EB0ull, // lua_settop
            0x141A12010ull, // lua_toboolean
            0x141A12070ull, // lua_tointeger
            0x141A120A0ull, // lua_tolstring
            0x141A12140ull, // lua_tonumber
            0x141A12250ull, // lua_type
            0x141A118B0ull, // lua_pushstring
            0x141A10DC0ull, // lua_createtable
            0x141A11A60ull, // lua_rawset
            0x141A11E80ull, // lua_settable
            0x141A11870ull, // lua_pushnil
            0x141A11540ull, // lua_next
            0x141A111F0ull, // lua_gettable
            0x141A11910ull, // lua_pushvalue
            0x141A11600ull, // lua_pcall
            0x141A116B0ull, // lua_pushcclosure
        };
        return value;
    }

    GameBuild DetectGameBuildFromVersionInfo(HMODULE hGame)
    {
        const std::wstring dir = GetModuleDirectory(hGame ? hGame : GetModuleHandleW(nullptr));
        if (dir.empty())
            return GameBuild::Unknown;

        const std::wstring versionInfoPath = dir + L"\\version_info.txt";
        std::string text = ReadWholeFileUtf8OrAnsi(versionInfoPath);
        if (text.empty())
        {
            Log("[AddressSet] Failed to read version_info.txt, defaulting to EN 1.0.15.4.\n");
            return GameBuild::En_1_0_15_4;
        }

        text = ToLowerAscii(text);
        Log("[AddressSet] version_info.txt = %s\n", text.c_str());

        const bool jp   = text.find("mst_jp") != std::string::npos;
        const bool prev = text.find("day1820") != std::string::npos;   // 1.0.15.3
        if (prev)
            return jp ? GameBuild::Jp_1_0_15_3 : GameBuild::En_1_0_15_3;
        return jp ? GameBuild::Jp_1_0_15_4 : GameBuild::En_1_0_15_4;    // day3800 / default = current
    }

    bool ResolveAddressSet(HMODULE hGame)
    {
        if (!hGame)
            return false;

        GetGameBuild() = DetectGameBuildFromVersionInfo(hGame);

        switch (GetGameBuild())
        {
        case GameBuild::En_1_0_15_3: GetAddressSet() = Get_mst_en_day1820_AddressSet(); break;
        case GameBuild::Jp_1_0_15_3: GetAddressSet() = Get_mst_jp_day1820_AddressSet(); break;
        case GameBuild::En_1_0_15_4: GetAddressSet() = Get_mst_en_day3800_AddressSet(); break;
        case GameBuild::Jp_1_0_15_4: GetAddressSet() = Get_mst_jp_day3800_AddressSet(); break;
        default:                     GetAddressSet() = Get_mst_en_day3800_AddressSet(); break;
        }
        Log("[AddressSet] Selected %s address set.\n", GetGameBuildName(GetGameBuild()));
        return true;
    }

    namespace
    {
        LPTOP_LEVEL_EXCEPTION_FILTER g_PrevCrashFilter = nullptr;

        void GetGameModuleRange(uintptr_t& base, uintptr_t& size)
        {
            base = 0; size = 0;
            HMODULE h = GetModuleHandleW(nullptr);
            if (!h) return;
            base = reinterpret_cast<uintptr_t>(h);
            const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(h);
            if (dos->e_magic != IMAGE_DOS_SIGNATURE) return;
            const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(
                reinterpret_cast<const BYTE*>(h) + dos->e_lfanew);
            if (nt->Signature != IMAGE_NT_SIGNATURE) return;
            size = nt->OptionalHeader.SizeOfImage;
        }

        const char* ModuleNameFromAddr(const void* addr, uintptr_t& base)
        {
            base = 0;
            HMODULE hm = nullptr;
            if (GetModuleHandleExA(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCSTR>(addr), &hm) && hm)
            {
                base = reinterpret_cast<uintptr_t>(hm);
                static char name[MAX_PATH];
                if (GetModuleFileNameA(hm, name, MAX_PATH))
                {
                    const char* slash = strrchr(name, '\\');
                    return slash ? slash + 1 : name;
                }
            }
            return "<unknown-module>";
        }

        LONG WINAPI HookSampleCrashFilter(EXCEPTION_POINTERS* ep)
        {
            __try
            {
                const EXCEPTION_RECORD* er = ep->ExceptionRecord;
                const CONTEXT* cx = ep->ContextRecord;
                const uintptr_t fault = reinterpret_cast<uintptr_t>(er->ExceptionAddress);

                uintptr_t gameBase = 0, gameSize = 0;
                GetGameModuleRange(gameBase, gameSize);

                uintptr_t modBase = 0;
                const char* modName = ModuleNameFromAddr(er->ExceptionAddress, modBase);

                CrashLogf("\n==================== HookSample CRASH ====================\n");
                CrashLogf("[CRASH] build=%s  exceptionCode=0x%08lX\n",
                          GetGameBuildName(GetGameBuild()),
                          static_cast<unsigned long>(er->ExceptionCode));
                CrashLogf("[CRASH] faulting instruction @ 0x%llX  (%s + 0x%llX)\n",
                          static_cast<unsigned long long>(fault), modName,
                          static_cast<unsigned long long>(modBase ? fault - modBase : 0ull));

                if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2)
                {
                    const ULONG_PTR kind = er->ExceptionInformation[0];
                    const char* op = (kind == 0) ? "READ" : (kind == 1) ? "WRITE"
                                   : (kind == 8) ? "EXECUTE" : "ACCESS";
                    CrashLogf("[CRASH] access violation: tried to %s 0x%llX\n",
                              op, static_cast<unsigned long long>(er->ExceptionInformation[1]));
                }

                const uintptr_t* vals = reinterpret_cast<const uintptr_t*>(&GetAddressSet());
                const int n = static_cast<int>(sizeof(AddressSet) / sizeof(uintptr_t));
                const char* bestName = nullptr; uintptr_t bestAddr = 0;
                for (int i = 0; i < n && i < kAddrFieldCount; ++i)
                {
                    const uintptr_t a = vals[i];
                    if (a && a <= fault && a > bestAddr) { bestAddr = a; bestName = kAddrFieldNames[i]; }
                }
                if (bestName)
                    CrashLogf("[CRASH] nearest hooked address at/below the fault: %s @ 0x%llX  (fault = %s + 0x%llX)\n",
                              bestName, static_cast<unsigned long long>(bestAddr),
                              bestName, static_cast<unsigned long long>(fault - bestAddr));
                else
                    CrashLogf("[CRASH] the fault is below every resolved hooked address.\n");

                CrashLogf("[CRASH] RIP=%016llX RSP=%016llX RBP=%016llX\n",
                          (unsigned long long)cx->Rip, (unsigned long long)cx->Rsp, (unsigned long long)cx->Rbp);
                CrashLogf("[CRASH] RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX\n",
                          (unsigned long long)cx->Rax, (unsigned long long)cx->Rbx,
                          (unsigned long long)cx->Rcx, (unsigned long long)cx->Rdx);
                CrashLogf("[CRASH] RSI=%016llX RDI=%016llX R8 =%016llX R9 =%016llX\n",
                          (unsigned long long)cx->Rsi, (unsigned long long)cx->Rdi,
                          (unsigned long long)cx->R8, (unsigned long long)cx->R9);
                CrashLogf("[CRASH] R10=%016llX R11=%016llX R12=%016llX R13=%016llX\n",
                          (unsigned long long)cx->R10, (unsigned long long)cx->R11,
                          (unsigned long long)cx->R12, (unsigned long long)cx->R13);
                CrashLogf("[CRASH] R14=%016llX R15=%016llX\n",
                          (unsigned long long)cx->R14, (unsigned long long)cx->R15);

                CrashLogf("[CRASH] stack return-address trail (game-module addrs == disassembly-dump addrs, no ASLR):\n");
                if (gameSize)
                {
                    const uintptr_t* sp = reinterpret_cast<const uintptr_t*>(cx->Rsp);
                    int shown = 0;
                    for (int i = 0; i < 256 && shown < 12; ++i)
                    {
                        const uintptr_t v = sp[i];
                        if (v >= gameBase && v < gameBase + gameSize)
                        {
                            CrashLogf("[CRASH]   [rsp+0x%03X] 0x%llX  (game+0x%llX)\n",
                                      i * 8, (unsigned long long)v,
                                      (unsigned long long)(v - gameBase));
                            ++shown;
                        }
                    }
                    if (shown == 0)
                        CrashLogf("[CRASH]   (no game-module return addresses in the first 2KB of stack)\n");
                }
                CrashLogf("[CRASH] Look up the faulting address / game+offset in this build's disassembly dump.\n");
                CrashLogf("===========================================================\n");
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                CrashLogf("[CRASH] (the crash logger itself faulted while writing the report)\n");
            }

            return g_PrevCrashFilter ? g_PrevCrashFilter(ep) : EXCEPTION_CONTINUE_SEARCH;
        }
    }

    void InstallCrashHandler()
    {
        g_PrevCrashFilter = SetUnhandledExceptionFilter(HookSampleCrashFilter);
        Log("[CRASH] Unhandled-exception crash logger installed.\n");
    }
}
