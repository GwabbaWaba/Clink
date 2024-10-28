#pragma once
#include <string>
#include "main.hpp"
#include "bitsize_ints.h"
using std::string;

// used in the registry to store strings related to a data piece
class RegistryNamespace {
    public:
    string name;

    virtual constexpr bool operator ==(const RegistryNamespace& other) const = default;
    virtual constexpr bool operator !=(const RegistryNamespace& other) const = default;

    constexpr RegistryNamespace(string name):
        name(name)
    {}
    virtual ~RegistryNamespace() = default;
};

class ModMemberNamespace : public RegistryNamespace {
    public:
    string mod_name;

    constexpr ModMemberNamespace(string mod_name, string name):
        RegistryNamespace(name),
        mod_name(mod_name)
    {}
};

// a more lightweight version of RegistryNamespace opting to only store id
class RegistryId {
    public:
    u32 id;
    
    constexpr bool operator ==(const RegistryId& other) const = default;
    constexpr bool operator !=(const RegistryId& other) const = default;

    constexpr RegistryId(u32 id):
        id(id)
    {}
    virtual ~RegistryId() = default;
};

class ModMemberId : public RegistryId {
    public:
    u32 mod_id;

    constexpr ModMemberId(u32 mod_id, u32 id):
        RegistryId(id),
        mod_id(mod_id)
    {}
};

namespace std {
    template <>
    struct hash<RegistryNamespace> {
        std::size_t operator()(const RegistryNamespace& x) const {
            return std::hash<string>()(x.name);
        }
    };

    template <>
    struct hash<ModMemberNamespace> {
        std::size_t operator()(const ModMemberNamespace& x) const {
            return std::hash<string>()(x.name) ^ (std::hash<string>()(x.mod_name) << 1);
        }
    };

    template <>
    struct hash<RegistryId> {
        std::size_t operator()(const RegistryId& x) const {
            return std::hash<u32>()(x.id);
        }
    };

    template <>
    struct hash<ModMemberId> {
        std::size_t operator()(const ModMemberId& x) const {
            return std::hash<u32>()(x.id) ^ (std::hash<u32>()(x.mod_id) << 1);
        }
    };
}

namespace registry_literals {
}
