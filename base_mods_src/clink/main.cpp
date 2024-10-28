#include <iostream>
#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include "../../src/registry.hpp"
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

class TestMod: public Mod {
public:
    void initialize() override {
        std::cout << "clink initialized!" << std::endl;
    }

    void shutdown() override {
        std::cout << "clink shut down!" << std::endl;
    }
};

clink Mod* createMod() {
    return new TestMod();
}
clink ModInfo* getModInfo() {
    return new ModInfo{
        .name = "clink"
    };
}
