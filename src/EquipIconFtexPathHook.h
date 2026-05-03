#pragma once

#include <cstdint>

bool Install_EquipIconFtexPath_Hook();
bool Uninstall_EquipIconFtexPath_Hook();

// Sets a per-equipId icon override using a raw .ftex path string.
// The path is hashed at registration time and looked up on each call.
void Set_EquipIconFtexPath(int equipId, const char* rawPath);

// Clears the override for a single equipId.
void Clear_EquipIconFtexPath(int equipId);

// Clears every registered override.
void Clear_AllEquipIconFtexPaths();
