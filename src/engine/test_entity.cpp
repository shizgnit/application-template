#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

#include <gtest/gtest.h>

// Dummy particle emitter class for test
class DummyEmitter {
public:
    bool triggered = false;
    void emit() { triggered = true; }
};

TEST(EntityTest, InstanceInstantiationDefault) {
    type::entity e;
    EXPECT_TRUE(e.empty());
    auto& inst = e.addInstance();
    EXPECT_FALSE(e.empty());
    EXPECT_EQ(e.size(), 1);
    EXPECT_TRUE(e.hasInstance(inst.id));
    EXPECT_EQ(inst.entity, &e);
}

TEST(EntityTest, InstanceInstantiationWithProperties) {
    type::entity e;
    properties props;
    props.set("test", 42);
    auto& inst = e.addInstance(props);
    EXPECT_TRUE(static_cast<properties&>(inst).has("test"));
    EXPECT_EQ(std::get<int>(static_cast<properties&>(inst).get("test")), 42);
    EXPECT_EQ(inst.entity, &e);
}

TEST(EntityTest, MultipleInstanceInstantiation) {
    type::entity e;
    int count = 5;
    properties props;
    auto& inst = e.addInstance(props, count);
    EXPECT_EQ(e.size(), count);
    for (auto& pair : e.getInstances()) {
        EXPECT_EQ(pair.second->entity, &e);
    }
}

TEST(EntityTest, ParticleEmitterManagement) {
    type::entity e;
    auto obj = std::make_shared<type::object>();
    e.setObject(obj);
    // The entity's object should be set and emitter assigned
    EXPECT_TRUE(e.object != nullptr);
    EXPECT_TRUE(obj->emitter == &e);
}

#endif
