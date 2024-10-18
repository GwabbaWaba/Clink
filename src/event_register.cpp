#include "event_register.hpp"

EventRegister::EventRegister():
    events(unordered_map<string, vector<VoidFn>>())
{}
