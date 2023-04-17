/*
================================================================================
  Copyright (c) 2023, Pandemos
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

#pragma once

namespace utilities {

    class test {
    public:
        typedef std::vector<value_t> parameters_t;

        static test& singleton() {
            static test *instance = new test();
            return *instance;
        }
        typedef std::function<void()> callback_t;

        int add(const std::string& group, const std::string& name, callback_t c) {
            callbacks[group][name] = c;
            static int count = 0;
            return ++count;
        }

        std::pair<bool, std::string> run(std::string input) {
            auto spec = utilities::tokenize(input, "::", 2);
            if (spec.size() != 2) {
                return { false, "failed to parse test spec" };
            }
            auto group = callbacks.find(spec[0]);
            if (group == callbacks.end()) {
                return { false, "failed to find test group " + spec[0] };
            }
            auto test = group->second.find(spec[1]);
            if (test == group->second.end()) {
                return { false, "failed to find test " + spec[1] + " in group " + spec[1] };
            }
            try {
                test->second();
            }
            catch (std::exception ex) {
                return { false, ex.what() };
            }
            return { true, "success" };
        }

        std::vector<std::string> list() {
            std::vector<std::string> results;
            for (auto group : callbacks) {
                for (auto test : group.second) {
                    results.push_back(group.first + "::" + test.first);
                }
            }
            return results;
        }

        std::string data(std::string subpath="") {
            std::string path = std::string();// +SOLUTION_PATH;
            if (subpath.empty() == false) {
                return(filesystem->join({ path, "tests", "data", subpath }));
            }
            else {
                return(filesystem->join({ path, "tests", "data" }));
            }
        }

    protected:
        std::map<std::string, std::map<std::string, callback_t>> callbacks;
    };

}

#define EXPECT_TRUE(_condition_) if ((_condition_) != true) { throw std::exception(); }
#define EXPECT_FALSE(_condition_) if ((_condition_) != false) { throw std::exception(); }

#define EXPECT(_condition_) EXPECT_TRUE(_condition_);
#define EXPECT_STREQ(_left_, _right_) EXPECT_TRUE(std::string(_left_).compare(std::string(_right_)) == 0)
#define EXPECT_STRNEQ(_left_, _right_) EXPECT_FALSE(std::string(_left_).compare(std::string(_right_)) == 0)
#define EXPECT_EQ(_left_, _right_) EXPECT_TRUE(_left_ == _right_)
#define EXPECT_NEQ(_left_, _right_) EXPECT_FALSE(_left_ == _right_)

#define TEST(_group_, _name_, _callback_) int _group_ ## _name_ = utilities::test::singleton().add(#_group_, #_name_, _callback_);
