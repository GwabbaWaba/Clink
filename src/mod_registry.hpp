#pragma once

#include "bitsizeints.h"
#include "clinkAPI.hpp"

#include <filesystem>
namespace fs = std::filesystem;

enum ModError: u8 {
    OK,
    LOAD_ERROR,
    REGISTRY_FUNCTION_ERROR,
    NO_MOD_GENERATED
};
typedef ModInfo* (*GetModInfoFn)();
struct ModData {
    Mod* mod;
    GetModInfoFn getModInfo;
    void* mod_handle;

    ModData(Mod* mod, GetModInfoFn getModInfo, void* mod_handle);
    ~ModData();
};
class ModRegister {
    public:
    unordered_map<string, ModData*> mods;

    ModRegister();
    ~ModRegister();
    ModError loadMod(string modPath, ClinkAPI* api);
    ModError loadMods(const fs::path& modDirPath, ClinkAPI* api);
};
