#include <iostream>

#include <filesystem>
namespace fs = std::filesystem;

#include "mod_registry.hpp"
#include "event_register.hpp"

#include "global.hpp"
#include "main.hpp"


ModRegister mod_register = ModRegister();
EventRegister event_register = EventRegister();

int main(i32 argc, char* argv[]) {
    i32 width = 800;
    i32 height = 600;
    raylib::InitWindow(width, height, "Clink");
    raylib::SetTargetFPS(60);
    
    auto api = ClinkAPI {
        .registerEvent = [](string eventName) {
            event_register.events[eventName] = vector<VoidFn>();
        },
        .getEvent = [](string eventName) {
            return event_register.events[eventName];
        },
        .subscribeToEvent = [](string eventName, VoidFn callback) {
            auto event = &event_register.events[eventName];
            event->push_back(callback);
            std::clog << "Subscribing " << (void*)callback << " to " << eventName << std::endl;
        },
    };
    api.registerEvent("clink::update");
    api.registerEvent("clink::draw");

    api.registerEvent("clink::key");

    for(const auto &entry : fs::recursive_directory_iterator("mods/")) {
        if(mod_register.loadMod(entry.path(), &api) != ModError::OK) {
            std::cerr << "Failed to load mod at: " << entry.path() << std::endl;
            return -1;
        }
    }

    while(!raylib::WindowShouldClose()) {
        for(auto callback : event_register.events["clink::update"]) {
            as(fn(void, f32), callback)(1.0);
        }

        raylib::BeginDrawing();
            ClearBackground(raylib::BLACK);

            for(auto callback : event_register.events["clink::draw"]) {
                as(fn(void), callback)();
            }
        raylib::EndDrawing();
    }

    return 0;
}
