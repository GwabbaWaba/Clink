#include <dlfcn.h>
#include <unordered_map>

#include "mod_registry.hpp"
#include "global.hpp"
#include "main.hpp"

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
    mods(unordered_map<string, ModData*>())
{}
ModRegister::~ModRegister() {
    for(auto &[name, data] : mods) {
        data->mod->shutdown();
        delete data;
    }
}

typedef Mod* (*CreateModFunc)();
typedef void (*InitializeAPIFunc)(ClinkAPI*);
ModError ModRegister::loadMod(string modPath, ClinkAPI* api) {
    Mod* mod = nullptr;
    void* mod_handle = dlopen(modPath.c_str(), RTLD_LAZY);
    if (!mod_handle) {
        println(dlerror());
        return ModError::LOAD_ERROR;
    }

    auto createMod = as(CreateModFunc, dlsym(mod_handle, "createMod"));
    auto getModInfo = as(GetModInfoFn, dlsym(mod_handle, "getModInfo"));
    auto initializeAPI = as(InitializeAPIFunc, dlsym(mod_handle, "initializeAPI"));

    if (!createMod || !getModInfo) {
        dlclose(mod_handle);
        println(dlerror());
        return ModError::REGISTRY_FUNCTION_ERROR;
    }

    // Create and use the mod
    mod = createMod();
    if (mod) {
        if (initializeAPI) {
            initializeAPI(api);
        }
        mod->initialize();
        mods[getModInfo()->name] = new ModData(mod, getModInfo, mod_handle);
        return ModError::OK;
    }

    dlclose(mod_handle);
    return ModError::NO_MOD_GENERATED;
}
