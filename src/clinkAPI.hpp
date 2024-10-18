#pragma once
#include <string>
#include <functional>

#include "clinkinterface.hpp"
using namespace std;

typedef void(*VoidFn)();
#define VOID_CAST(return, args...) (VoidFnPtr)(void(*return)(args))

struct ClinkAPI {
    function<void(string /*eventName*/)> registerEvent;
    function<const vector<VoidFn>&(string /*eventName*/)> getEvent;
    function<void(string /*eventName*/, VoidFn)> subscribeToEvent;
};

union UnicodeChar {
    unsigned int  value;
    unsigned char bytes[4];
};
clink void initializeAPI(ClinkAPI* clinkAPI);
