#include "../../src/clinkinterface.hpp"
#include "../../src/clinkAPI.hpp"
#include "../../src/registry.hpp"
#include "../../src/main.hpp"

using namespace std;
using namespace registry_literals;

ClinkAPI* api;
clink void initializeAPI(ClinkAPI* clink_API) {
    api = clink_API;
}

void update(f32 dt) {
    auto player = api->getPlayerAPI();
    if(rl::IsKeyDown(rl::KEY_W))
        player->moveY(-3.0);
    if(rl::IsKeyDown(rl::KEY_A))
        player->moveX(-3.0);
    if(rl::IsKeyDown(rl::KEY_S))
        player->moveY(3.0);
    if(rl::IsKeyDown(rl::KEY_D))
        player->moveX(3.0);
}

class TestMod: public Mod {
public:
    void initialize() override {
        auto update_event_id = api->registerEvent("clink::update"_ModMember);
        auto draw_event_id = api->registerEvent("clink::draw"_ModMember);
        api->subscribeToEvent("clink::update"_ModMember, (VoidFn)update);
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
