#include <iostream>
#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
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
        clinkAPI->subscribeToEvent("clink::update", (VoidFn)update);
        clinkAPI->subscribeToEvent("clink::draw", (VoidFn)draw);
        clinkAPI->subscribeToEvent("clink::key", (VoidFn)key);

        std::cout << "TestMod initialized!" << std::endl;
    }

    void shutdown() override {
        std::cout << "TestMod shut down!" << std::endl;
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
