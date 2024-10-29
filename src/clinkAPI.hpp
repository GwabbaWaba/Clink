#pragma once
#include <string>
#include <optional>
#include <functional>

#include "clinkinterface.hpp"
#include "player.hpp"
#include "registry.hpp"

using std::string;
using std::optional;

typedef void(*VoidFn)();
#define VOID_CAST(return, args...) (VoidFnPtr)(void(*return)(args))

class EventRegister;
class ModRegister;
struct ClinkAPI {
    ClinkAPI(PlayerAPI* player_API, EventRegister& event_register, ModRegister& mod_register);

    // getPlayerAPI
    // returns PlayerAPI*
    std::function<PlayerAPI*()> getPlayerAPI;

    // registerEvent
    // ModMemberNamespace& event_name
    // returns ModMemberId, being the id of the registered event
    std::function<ModMemberId(ModMemberNamespace)> registerEvent;

    // getEvent
    // ModMemberId& event_id
    // returns std::vector<VoidFn>const&, being the list of callbacks registered to the event
    std::function<std::vector<VoidFn>&(ModMemberId&)> getEvent;

    // getEventByName
    // ModMemberNamespace& event_name
    // returns std::vector<VoidFn>const&, being the list of callbacks registered to the event
    std::function<std::vector<VoidFn>&(ModMemberNamespace)> getEventByName;

    // getEventIdByName
    // ModMemberNamespace& event_name
    // returns optional<ModMemberId>
    std::function<optional<ModMemberId>(ModMemberNamespace)> getEventIdByName;

    // subscribeToEvent
    // ModMemberNamespace& event_name
    // VoidFn callback
    // returns void
    std::function<void(ModMemberNamespace, VoidFn)> subscribeToEvent;
};

union UnicodeChar {
    unsigned int  value;
    unsigned char bytes[4];
};
clink void initializeAPI(ClinkAPI* clink_API);
