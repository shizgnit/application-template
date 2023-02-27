#pragma once

namespace utilities {

    class test {
    public:
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

        void run(std::string subset="") {
            for (auto group : callbacks) {
                if (subset.empty() == false && group.first != subset) {
                    continue;
                }
                for (auto test : callbacks[group.first]) {
                    try {
                        test.second();
                        // success
                    }
                    catch (...) {
                        // failure
                    }
                }
            }
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