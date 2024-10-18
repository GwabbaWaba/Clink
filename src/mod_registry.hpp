#pragma once

#include "global.hpp"

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
};
