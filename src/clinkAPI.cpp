#include <iostream>
#include "clinkAPI.hpp"
#include "event_register.hpp"
#include "main.hpp"
#include "mod_registry.hpp"
#include "player.hpp"
#include "registry.hpp"

ClinkAPI::ClinkAPI(PlayerAPI* player_API, EventRegister& event_register, ModRegister& mod_register) {
    this->getPlayerAPI = [player_API]() -> PlayerAPI* {
        return player_API;
    };
    this->registerEvent = [&](ModMemberNamespace event_name) -> ModMemberId {
        auto event_id = this->getEventIdByName(event_name);
        if(event_id == std::nullopt) {
            auto mod_id = mod_register.mod_ids.at(RegistryNamespace(event_name.mod_name));
            return event_register.registerEvent(event_name, mod_id);
        }

        std::cerr
            << "Duplicate event registry, errors may occur: "
            << event_name.mod_name
            << "::"
            << event_name.name
            << debug::endl;
        return event_id.value();
    };
    this->getEvent = [&](ModMemberId& event_id) -> std::vector<VoidFn>& {
        return event_register.events.at(event_id);
    };
    this->getEventByName = [&](ModMemberNamespace event_name) -> std::vector<VoidFn>& {
        return event_register.events.at(event_register.event_ids.at(event_name));
    };
    this->getEventIdByName = [&](ModMemberNamespace event_name) -> optional<ModMemberId> {
        auto events = event_register.event_ids;
        if(events.find(event_name) == events.end())
            return {};

        return event_register.event_ids.at(event_name);
    };
    this->subscribeToEvent = [&](ModMemberNamespace event_name, VoidFn callback) {
        auto& event = this->getEventByName(event_name);
        event.push_back(callback);
        debug::print_stream()
            << "Subscribing "
            << (void*)callback
            << " to "
            << event_name.mod_name
            << "::"
            << event_name.name
            << debug::endl;
    };
}
