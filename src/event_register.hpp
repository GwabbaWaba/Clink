#pragma once

#include "clinkAPI.hpp"
using std::string;

class EventRegister {
    public:
    std::unordered_map<string, std::vector<VoidFn>> events;

    EventRegister();
};
