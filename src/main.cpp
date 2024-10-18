#include <math.h>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <raylib.h>

#include <gwabbacpp>
#include "main.hpp"
#include "clinkinterface.hpp"
#include "clinkAPI.hpp"

using namespace std;
namespace fs = std::filesystem;
#define HashMap unordered_map


enum ModError: u8 {
    OK,
    LOAD_ERROR,
    REGISTRY_FUNCTION_ERROR,
    NO_MOD_GENERATED
};

typedef Mod* (*CreateModFunc)();
typedef ModInfo* (*GetModInfoFunc)();
typedef void (*InitializeAPIFunc)(ClinkAPI*);
struct ModData {
    Mod* mod;
    GetModInfoFunc getModInfo;
    void* modHandle;

    ModData(Mod* mod, GetModInfoFunc getModInfo, void* modHandle):
        mod(mod),
        getModInfo(getModInfo),
        modHandle(modHandle)
    {}
    ~ModData() {
        delete mod;
        dlclose(modHandle);
    }
};

typedef HashMap<string, ModData*> ModRegister;
typedef HashMap<string, vector<VoidFnPtr>> EventRegister;
class Game {
    public:
    Game():
        mods(ModRegister()),
        events(EventRegister())
    {}
    ~Game() {
        for(auto &[name, data] : mods) {
            data->mod->shutdown();
            delete data;
        }
    }

    ModError loadMod(string modPath, ClinkAPI* api) {
        Mod* mod = nullptr;
        void* modHandle = dlopen(modPath.c_str(), RTLD_LAZY);
        if (!modHandle) {
            std::cerr << dlerror() << std::endl;
            return ModError::LOAD_ERROR;
        }

        CreateModFunc createMod = (CreateModFunc)dlsym(modHandle, "createMod");
        GetModInfoFunc getModInfo = (GetModInfoFunc)dlsym(modHandle, "getModInfo");
        InitializeAPIFunc initializeAPI = (InitializeAPIFunc)dlsym(modHandle, "initializeAPI");

        if (!createMod || !getModInfo) {
            dlclose(modHandle);
            std::cerr << dlerror() << std::endl;
            return ModError::REGISTRY_FUNCTION_ERROR;
        }

        // Create and use the mod
        mod = createMod();
        if (mod) {
            if (initializeAPI) {
                initializeAPI(api);
            }
            mod->initialize();
            mods[getModInfo()->name] = new ModData(mod, getModInfo, modHandle);
            return ModError::OK;
        }

        dlclose(modHandle);
        return ModError::NO_MOD_GENERATED;
    }

    ModRegister mods;
    EventRegister events;
};

Game game = Game();

int main(i32 argc, char* argv[]) {
    i32 width = 800;
    i32 height = 600;
    InitWindow(width, height, "raylib [core] example - basic window");
    SetTargetFPS(60);
    
    let api = ClinkAPI {
        .registerEvent = [](string eventName) {
            game.events[eventName] = vector<VoidFnPtr>();
        },
        .getEvent = [](string eventName) {
            return game.events[eventName];
        },
        .subscribeToEvent = [](string eventName, VoidFnPtr callback) {
            let event = &game.events[eventName];
            event->push_back(callback);
            std::cout << "Subscribing " << (void*)callback << " to " << eventName << std::endl;
        },
    };
    api.registerEvent("clink::update");
    api.registerEvent("clink::draw");

    api.registerEvent("clink::key");

    for(const auto &entry : fs::recursive_directory_iterator("mods/")) {
        if(game.loadMod(entry.path(), &api) != ModError::OK) return -1;
    }

    while(!WindowShouldClose()) {
        for(auto &callback : game.events["clink::update"]) {
            let castedCallback = (void(*)(f32))callback;
            castedCallback(1.0);
        }

        for(auto &callback : game.events["clink::draw"]) {
            let castedCallback = (void(*)())callback;
            castedCallback();
        }
    }

    return 0;
}
