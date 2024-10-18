#include <iostream>
#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include <fstream>
#include <sstream>
#include <gwabbacpp>
using namespace std;

ClinkAPI* clinkAPI;
clink void initializeAPI(ClinkAPI* api) {
    clinkAPI = api;
}

void update(f32 dt) {
}

void draw() {
}

void key(UnicodeChar codepoint) {
    cout << codepoint.bytes << endl;
}

class TestMod: public Mod {
public:
    void initialize() override {
        clinkAPI->subscribeToEvent("clink::update", (VoidFnPtr)update);
        clinkAPI->subscribeToEvent("clink::draw", (VoidFnPtr)draw);
        clinkAPI->subscribeToEvent("clink::key", (VoidFnPtr)key);

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
