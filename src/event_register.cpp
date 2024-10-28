#include "event_register.hpp"
#include "registry.hpp"
using std::string;

EventRegister::EventRegister():
    event_ids(std::unordered_map<ModMemberNamespace, ModMemberId>()),
    events(std::unordered_map<ModMemberId, std::vector<VoidFn>>())

{}

ModMemberId EventRegister::registerEvent(ModMemberNamespace& event_name, RegistryId& mod_id) {
    auto id = ModMemberId(mod_id.id, this->next_id);
    this->next_id++;
    this->event_ids.emplace(event_name, id);
    this->events.emplace(id, std::vector<VoidFn>());
    return id;
}
