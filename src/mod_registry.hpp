#pragma once

#include "bitsize_ints.h"
#include "clinkAPI.hpp"
#include "registry.hpp"

#include <filesystem>
namespace fs = std::filesystem;
using std::string;

enum ModError: u8 {
    OK,
    LOAD_ERROR,
    REGISTRY_FUNCTION_ERROR,
    NO_MOD_GENERATED,
    INVALID_MOD_NAME
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
    private:
    u32 next_id = 0;
    public:
    std::unordered_map<RegistryNamespace, RegistryId> mod_ids;
    std::unordered_map<RegistryId, ModData*> mods;

    ModRegister();
    ~ModRegister();
    ModError loadMod(fs::path mod_path, ClinkAPI* api);
    ModError loadMods(const fs::path& mod_dir_path, ClinkAPI* api);
    ModError registerMod(Mod* mod, ModData* mod_data);
};
