#include "mod/MyMod.h"
#include "ll/api/Config.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/mod/RegisterHelper.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/ActorDefinitionIdentifier.h"
#include "mc/world/actor/FishingHook.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/item/ItemActor.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/registry/ItemStack.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/Spawner.h"
#include "mc/world/level/dimension/Dimension.h"
#include <algorithm>
#include <ll/api/memory/Hook.h>
#include <ll/api/service/Bedrock.h>
#include <mc/nbt/CompoundTag.h>
#include <memory>
#include <random>
#include <vector>


using string = std::string;
namespace fs = std::filesystem;


enum class SpawnType : int { Mob, Item, None };
struct ListItem {
    SpawnType type;
    float     probability{0.01f};
    string    data{""};
    bool      isSNBT{false};
};
using List = std::vector<ListItem>;
struct Config {
    int  version = 1;
    List lists   = {
        {SpawnType::None, 0.5f}, // 原版物品
        {SpawnType::Mob, 0.15f, "minecraft:creeper"}
    };
} cfg;


const ListItem& getRandomItem(const List& list = cfg.lists) {
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    static std::vector<float>               cumulativeProbabilities;
    static size_t                           lastSize = 0;

    if (list.size() != lastSize) {
        lastSize = list.size();
        cumulativeProbabilities.clear();
        cumulativeProbabilities.reserve(list.size());
        float totalProbability = 0.0f;
        for (const auto& item : list) {
            totalProbability += item.probability;
            cumulativeProbabilities.push_back(totalProbability);
        }
    }

    float  randomValue = dis(gen) * cumulativeProbabilities.back();
    auto   it          = std::lower_bound(cumulativeProbabilities.begin(), cumulativeProbabilities.end(), randomValue);
    size_t index       = std::distance(cumulativeProbabilities.begin(), it);

    return list[index];
}


LL_AUTO_INSTANCE_HOOK(
    PullClloserHook,
    ll::memory::HookPriority::High,
    "?_pullCloser@FishingHook@@IEAAXAEAVActor@@M@Z",
    void,
    Actor& a1,
    float  speed
) {
    try {
        auto const pos     = a1.getPosition();
        auto&      bs      = a1.getDimensionBlockSource();
        auto&      spawner = a1.getLevel().getSpawner();

        auto const& rd = getRandomItem();

        Actor* actor = nullptr;

        if (rd.type == SpawnType::Mob) {
            ActorDefinitionIdentifier id(rd.data);
            actor = spawner.spawnMob(bs, id, nullptr, pos, false, true, false);
        } else if (rd.type == SpawnType::Item) {
            ItemStack stack;
            if (rd.isSNBT) {
                auto snbt = CompoundTag::fromSnbt(rd.data);
                if (snbt.has_value()) {
                    stack = ItemStack::fromTag(*snbt->clone());
                } else my_plugin::MyMod::getInstance().getSelf().getLogger().error("Invalid SNBT: {}", rd.data);
            } else {
                stack = ItemStack(rd.data);
            }
            actor = spawner.spawnItem(bs, stack, nullptr, pos);
        } else {
            origin(a1, speed); // 兼容原版
            return;
        }

        if (actor) {
            a1.despawn();
            origin(*actor, speed);
            return;
        }
        origin(a1, speed); // 使用原实体
    } catch (...) {
        origin(a1, speed); // 使用原实体
        my_plugin::MyMod::getInstance().getSelf().getLogger().fatal(
            "Fail in PullClloserHook, please check your config.json"
        );
    }
}


namespace my_plugin {

static std::unique_ptr<MyMod> instance;

MyMod& MyMod::getInstance() { return *instance; }

bool MyMod::load() {
    auto& logger = getSelf().getLogger();
    logger.info("Loading...");
    logger.info("Author: engsr6982");
    logger.info("LICENSE: GPLv3");

    fs::path p = getSelf().getConfigDir() / "config.json";

    if (!fs::exists(p)) {
        ll::config::saveConfig(cfg, p);
    }

    bool ok = ll::config::loadConfig(cfg, p);
    if (!ok) ll::config::saveConfig(cfg, p);

    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().info("Enabling...");
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().info("Disabling...");
    return true;
}

} // namespace my_plugin

LL_REGISTER_MOD(my_plugin::MyMod, my_plugin::instance);
