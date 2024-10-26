#include "event_register.hpp"
using std::string;

EventRegister::EventRegister():
    events(std::unordered_map<string, std::vector<VoidFn>>())
{}
