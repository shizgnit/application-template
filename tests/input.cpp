#include "gtest/gtest.h"

#include "tests.hpp"

std::vector<platform::input::event> events;

class InputTest : public ::testing::Test {
protected:
    void SetUp() override {
        events.clear();
    }
    // void TearDown() override {}
};

TEST_F(InputTest, PointerDirect) {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event &ev) {
        events.push_back(ev);
    }, 1);

    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, { 1.0f, 1.0f, 1.0f }, 2 });

    EXPECT_EQ(events.size(), 1);
    delete input;
}

TEST_F(InputTest, PointerDrag) {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::DRAG, [](const platform::input::event& ev) {
        events.push_back(ev);
    }, 1);

    input->on_press({ platform::input::POINTER, platform::input::DOWN, 1, { 0.0f, 0.0f, 0.0f }, 2 });
    input->on_move({ platform::input::POINTER, platform::input::MOVE, 1, { 1.0f, 1.0f, 1.0f }, 3 });
    input->on_move({ platform::input::POINTER, platform::input::MOVE, 1, { 1.0f, 1.0f, 1.0f }, 4 });
    input->on_release({ platform::input::POINTER, platform::input::UP, 1, { 0.0f, 0.0f, 0.0f }, 2 });

    // Should not be a DRAG action without a index
    input->on_move({ platform::input::POINTER, platform::input::MOVE, 1, { 1.0f, 1.0f, 1.0f }, 4 });

    EXPECT_EQ(events.size(), 2);
    delete input;
}