#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "bitsize_ints.h"
#include "registry.hpp"
using namespace bitsize_literals;

using std::string;

struct ItemType {
    ModMemberId ids;
    u32 max_stack = 0_u32 - 1_u32;
};

class ItemRegistry {
    public:
    std::unordered_map<ModMemberNamespace, ModMemberId> item_ids;
    std::unordered_map<ModMemberId, ItemType> item_types;
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
