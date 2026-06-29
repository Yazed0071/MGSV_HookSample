#include "pch.h"

#include "AddressSet.h"

namespace AddressSetRuntime
{
    const AddressSet& Get_mst_en_day1820_AddressSet()   // 1.0.15.3 english
    {
        static const AddressSet value =
        {
            0x145E5EE70ull, // GetCurrentMissionCode
            0x145E62540ull, // GetIconFtexPath
            0x146ACC650ull, // ShlnSetFovaImpl

            0x1408D78A0ull, // SetLuaFunctions
            0x14006B6D0ull, // FoxLuaRegisterLibrary
            0x14C1D7320ull, // lua_getfield
            0x14C1D7D40ull, // lua_gettop
            0x14C1D8C90ull, // lua_isnumber
            0x14C1D9250ull, // lua_isstring
            0x14C1DA960ull, // lua_objlen
            0x14C1DB230ull, // lua_pushboolean
            0x141A11BC0ull, // lua_pushnumber
            0x14C1E9320ull, // lua_rawgeti
            0x14C1EBBE0ull, // lua_settop
            0x141A12330ull, // lua_toboolean
            0x141A12390ull, // lua_tointeger
            0x141A123C0ull, // lua_tolstring
            0x141A12460ull, // lua_tonumber
            0x14C1ED760ull, // lua_type
            0x14C1E7EE0ull, // lua_pushstring
            0x14C1D6320ull, // lua_createtable
            0x14C1E9CF0ull, // lua_rawset
            0x14C1EB2B0ull, // lua_settable
            0x14C1E7CC0ull, // lua_pushnil
            0x14C1DA770ull, // lua_next
            0x14C1D7C10ull, // lua_gettable
            0x14C1E87E0ull, // lua_pushvalue
            0x141A11930ull, // lua_pcall
            0x14C1E67B0ull, // lua_pushcclosure
        };
        return value;
    }

    const AddressSet& Get_mst_jp_day1820_AddressSet()   // 1.0.15.3 japanese
    {
        static const AddressSet value =
        {
            0x147A691E0ull, // GetCurrentMissionCode
            0x147A6BD40ull, // GetIconFtexPath
            0x148656360ull, // ShlnSetFovaImpl

            0x1408D72B0ull, // SetLuaFunctions
            0x1431CC520ull, // FoxLuaRegisterLibrary
            0x14C987300ull, // lua_getfield
            0x14C987CB0ull, // lua_gettop
            0x14C988960ull, // lua_isnumber
            0x14C988CA0ull, // lua_isstring
            0x14C98A230ull, // lua_objlen
            0x14C98B310ull, // lua_pushboolean
            0x14C98D800ull, // lua_pushnumber
            0x14C98EBC0ull, // lua_rawgeti
            0x14C990ED0ull, // lua_settop
            0x14C991120ull, // lua_toboolean
            0x14C991B80ull, // lua_tointeger
            0x14C992060ull, // lua_tolstring
            0x14C9924D0ull, // lua_tonumber
            0x14C9935F0ull, // lua_type
            0x14C98DCB0ull, // lua_pushstring
            0x14C986520ull, // lua_createtable
            0x14C98ED50ull, // lua_rawset
            0x14C990BD0ull, // lua_settable
            0x14C98D570ull, // lua_pushnil
            0x14C98A010ull, // lua_next
            0x14C987B90ull, // lua_gettable
            0x14C98E1D0ull, // lua_pushvalue
            0x141A11A50ull, // lua_pcall
            0x14C98C080ull, // lua_pushcclosure
        };
        return value;
    }
}
