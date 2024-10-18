#pragma once

#include "clinkAPI.hpp"

class EventRegister {
    public:
    unordered_map<string, vector<VoidFn>> events;

    EventRegister();
};
