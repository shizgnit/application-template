
#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

// GoogleTest unit tests for platform::assets
#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <string>

using namespace platform;

// Minimal mock subclass for testing abstract methods
class MockAssets : public assets {
public:
    std::vector<identifier_t> list(const std::string& path, const std::string& type = "") override {
        return {"asset1", "asset2"};
    }
    std::istream& retrieve(const std::string& path) override {
        std::stringstream* ss = new std::stringstream;
        *ss << "data for " << path;
        _stack.push_back({path, ss});
        return *ss;
    }
    void release() override {
        if (!_stack.empty()) {
            std::stringstream* ss = static_cast<std::stringstream*>(_stack.back().stream);
            delete ss;
            _stack.pop_back();
        }
    }
    std::string load(const std::string& type, const std::string& resource, const identifier_t& id = "") override {
        return type + ":" + resource + ":" + id;
    }

    // Move _stack to public for testing
    using assets::_stack;
};

// Dummy type for cache testing
struct DummyType : public type::info {
    std::string type() const { return "DummyType"; }
public:
    using type::info::_id;
};

TEST(AssetsTest, ConstructionAndDestruction) {
    MockAssets a;
    EXPECT_TRUE(a._stack.empty());
}

TEST(AssetsTest, ListReturnsAssets) {
    MockAssets a;
    auto assetsList = a.list("/test");
    EXPECT_EQ(assetsList.size(), 2);
    EXPECT_EQ(assetsList[0], "asset1");
}

TEST(AssetsTest, RetrieveAndRelease) {
    MockAssets a;
    std::istream& stream = a.retrieve("/file");
    std::string data;
    stream >> data;
    EXPECT_EQ(data, "data");
    a.release();
    EXPECT_TRUE(a._stack.empty());
}

TEST(AssetsTest, LoadReturnsExpectedString) {
    MockAssets a;
    std::string result = a.load("typeA", "resourceB", "idC");
    EXPECT_EQ(result, "typeA:resourceB:idC");
}

TEST(AssetsTest, CacheCreateReferenceHasGet) {
    MockAssets a;
    auto id = "dummy_id";
    auto ptr = a.create<DummyType>(id);
    EXPECT_TRUE(ptr != nullptr);
    EXPECT_EQ(ptr->_id, id);
    EXPECT_TRUE(a.has<DummyType>(id));
    auto ref = a.reference<DummyType>(id);
    EXPECT_EQ(ref->_id, id);
    DummyType& obj = a.get<DummyType>(id);
    EXPECT_EQ(obj._id, id);
    
    //EXPECT_FALSE(a.has<DummyType>(id));
}

TEST(AssetsTest, GetAllReturnsAllCached) {
    MockAssets a;
    auto ids = std::vector<std::string>{"id1", "id2"};
    for (const auto& id : ids) {
        a.create<DummyType>(id);
    }
    auto all = a.get<DummyType>();
    EXPECT_EQ(all.size(), 2);
    EXPECT_EQ(all[0]->_id, "id1");
    EXPECT_EQ(all[1]->_id, "id2");
}

#endif
