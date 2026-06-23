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
        };
        return value;
    }
}
