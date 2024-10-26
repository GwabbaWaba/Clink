#pragma once
#include <string>
#include <functional>

#include "clinkinterface.hpp"

using std::string;

typedef void(*VoidFn)();
#define VOID_CAST(return, args...) (VoidFnPtr)(void(*return)(args))

struct ClinkAPI {
    // void registerEvent
    // string event_name
    std::function<void(string)> registerEvent;

    // const std::vector<VoidFn> getEvent
    // string event_name
    std::function<const std::vector<VoidFn>&(string)> getEvent;

    // void subscribeToEvent
    // string event_name
    // VoidFn callback
    std::function<void(string, VoidFn)> subscribeToEvent;
};

union UnicodeChar {
    unsigned int  value;
    unsigned char bytes[4];
};
clink void initializeAPI(ClinkAPI* clink_API);
