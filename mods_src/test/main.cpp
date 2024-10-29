#include "../../src/raylib.h"

#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include "../../src/registry.hpp"
#include "../../src/main.hpp"
#include "../../src/bitsize_ints.h"

namespace rl = raylib;
using namespace registry_literals;

ClinkAPI* clinkAPI;
clink void initializeAPI(ClinkAPI* api) {
    clinkAPI = api;
}

void update(f32 dt) {
}

void draw() {
}

class TestMod: public Mod {
public:
    void initialize() override {
        clinkAPI->subscribeToEvent("clink::update"_ModMember, (VoidFn)update);
        clinkAPI->subscribeToEvent("clink::draw"_ModMember, (VoidFn)draw);

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
