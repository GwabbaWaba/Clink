#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include "../../src/main.hpp"

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
        debug::println("clink initialized!");
    }

    void shutdown() override {
        debug::println("clink shut down!");
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
