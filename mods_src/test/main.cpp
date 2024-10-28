#include <iostream>
#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include "../../src/registry.hpp"
#include "../../src/main.hpp"
#include "../../src/bitsize_ints.h"

#include <math.h>
namespace raylib {
    #include <raylib.h>
    #include <raymath.h>
}
using namespace std;

ClinkAPI* clinkAPI;
clink void initializeAPI(ClinkAPI* api) {
    clinkAPI = api;
}

void update(f32 dt) {
}

void draw() {
    raylib::DrawText("Hi!", 190, 200, 40, raylib::WHITE);
}

void key(UnicodeChar codepoint) {
    cout << codepoint.bytes << endl;
}

class TestMod: public Mod {
public:
    void initialize() override {
        clinkAPI->subscribeToEvent(ModMemberNamespace("clink", "update"), (VoidFn)update);
        clinkAPI->subscribeToEvent(ModMemberNamespace("clink", "draw"), (VoidFn)draw);
        clinkAPI->subscribeToEvent(ModMemberNamespace("clink", "key_press"), (VoidFn)key);

        debug::println("TestMod initialized!");
    }

    void shutdown() override {
        debug::println("TestMod shut down!");
    }
};

clink Mod* createMod() {
    return new TestMod();
}
clink ModInfo* getModInfo() {
    return new ModInfo{
        .name = "TestMod"
    };
}
