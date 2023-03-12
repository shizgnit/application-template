/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

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

    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 2, 0.0f, { 1.0f, 1.0f, 1.0f } });

    EXPECT_EQ(events.size(), 1);
    delete input;
}

TEST_F(InputTest, PointerDrag) {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::DRAG, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 3, 0.0f, { 1.0f, 1.0f, 1.0f } });
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 4, 0.0f, { 1.0f, 1.0f, 1.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });

    // Should not be a DRAG action without a index
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 4, 0.0f, { 1.0f, 1.0f, 1.0f } });

    EXPECT_EQ(events.size(), 3);
    delete input;
}

TEST_F(InputTest, PointerDoubleTap) {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->handler(platform::input::POINTER, platform::input::DOUBLE, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 3, 0.0f, { 0.0f, 0.1f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 3, 0.0f, { 0.0f, 0.1f, 0.0f } });

    EXPECT_EQ(events.size(), 2);
    delete input;
}