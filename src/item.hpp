#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bitsize_ints.h"
using namespace bitsize_literals;

using std::string;

struct RegistryNamespace {
    const string mod_name; // mod this thing originates from
    const string name; // name of the thing

    constexpr bool operator ==(const RegistryNamespace& other) const = default;
    constexpr bool operator !=(const RegistryNamespace& other) const = default;
};

struct ItemType {
    const u32 type_id;
    u32 max_stack = 0_u32 - 1_u32;

    RegistryNamespace registry_namespace;
};

class ItemRegistry {
    public:
    std::unordered_map<u32, ItemType> item_types;
    
    ItemType& namespaceLookup(const RegistryNamespace& name_space);
};

struct ItemStack {
    const u32 type_id;
    u32 size;
};

class InventorySlot {
    u32 max_stack = 0_u32 - 1_u32;
    std::optional<ItemStack> stack;
};

class Inventory {
    private:
    u32 max_slots;
    std::vector<InventorySlot> slots;

    public:
    Inventory()
    {}
};
