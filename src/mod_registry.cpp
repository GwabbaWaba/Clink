#include <iostream>
#include <dlfcn.h>
#include <unordered_map>

#include "mod_registry.hpp"
#include "clinkAPI.hpp"
#include "clinkinterface.hpp"
#include "main.hpp"
#include "registry.hpp"

#include <filesystem>
namespace fs = std::filesystem;
using std::string;

ModData::ModData(Mod* mod, GetModInfoFn getModInfo, void* mod_handle):
    mod(mod),
    getModInfo(getModInfo),
    mod_handle(mod_handle)
{}
ModData::~ModData() {
    delete mod;
    dlclose(mod_handle);
}

ModRegister::ModRegister():
    mod_ids(std::unordered_map<RegistryNamespace, RegistryId>()),
    mods(std::unordered_map<RegistryId, ModData*>())
{}
ModRegister::~ModRegister() {
    for(auto &[name, data] : mods) {
        data->mod->shutdown();
        delete data;
    }
}

typedef Mod* (*CreateModFunc)();
typedef void (*InitializeAPIFunc)(ClinkAPI*);
ModError ModRegister::loadMod(fs::path mod_path, ClinkAPI* api) {
    Mod* mod = nullptr;
    void* mod_handle = dlopen(mod_path.c_str(), RTLD_LAZY);
    if (!mod_handle) {
        debug::println(dlerror());
        return ModError::LOAD_ERROR;
    }

    auto createMod = as(CreateModFunc, dlsym(mod_handle, "createMod"));
    auto getModInfo = as(GetModInfoFn, dlsym(mod_handle, "getModInfo"));
    auto initializeAPI = as(InitializeAPIFunc, dlsym(mod_handle, "initializeAPI"));

    if (!createMod || !getModInfo) {
        dlclose(mod_handle);
        debug::println(dlerror());
        return ModError::REGISTRY_FUNCTION_ERROR;
    }

    // Create and use the mod
    mod = createMod();
    if (mod) {
        if (initializeAPI) {
            initializeAPI(api);
        }
        mod->initialize();
        this->registerMod(mod, new ModData(mod, getModInfo, mod_handle));
        return ModError::OK;
    }

    dlclose(mod_handle);
    return ModError::NO_MOD_GENERATED;
}

ModError ModRegister::loadMods(const fs::path& mod_dir_path, ClinkAPI* api) {
    for(const auto &entry : fs::recursive_directory_iterator(mod_dir_path)) {
        auto result = this->loadMod(entry.path(), api);
        if(result != ModError::OK) {
            std::cerr << "Failed to load mod at: " << entry.path() << std::endl;
            return result;
        }
    }
    return ModError::OK;
}

void ModRegister::registerMod(Mod* mod, ModData* mod_data) {
    auto id = RegistryId(this->next_id);
    this->next_id++;
    this->mod_ids.emplace(RegistryNamespace(mod_data->getModInfo()->name), id);
    this->mods.emplace(id, mod_data);
    
}
