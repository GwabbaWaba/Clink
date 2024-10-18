#pragma once
#include <string>
#include <functional>

#include "clinkinterface.hpp"
using namespace std;

typedef void(*VoidFnPtr)();
#define VOID_CAST(return, args...) (VoidFnPtr)(void(*return)(args))

struct ClinkAPI {
    function<void(string /*eventName*/)> registerEvent;
    function<const vector<VoidFnPtr>&(string /*eventName*/)> getEvent;
    function<void(string /*eventName*/, VoidFnPtr)> subscribeToEvent;
};

union UnicodeChar {
    unsigned int  value;
    unsigned char bytes[4];
};
clink void initializeAPI(ClinkAPI* clinkAPI);
