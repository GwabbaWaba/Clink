#pragma once

#include "clinkAPI.hpp"
#include "registry.hpp"
#include "bitsize_ints.h"
using std::string;

class EventRegister {
    private:
    u32 next_id = 0;
    public:
    std::unordered_map<ModMemberNamespace, ModMemberId> event_ids;
    std::unordered_map<ModMemberId, std::vector<VoidFn>> events;

    EventRegister();
    ModMemberId registerEvent(ModMemberNamespace& event_name, RegistryId& mod_id);
};
