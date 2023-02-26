#pragma once

namespace utilities {

    class test {
        public:
            static TestManager &singleton() {
                static TestManager instance;
                return instance; 
            }
            typedef std::function<void()> callback_t;

            void add(const std::string &group, const std::string &name, callback_t c) {
                callbacks[group][name] = c;
            }

            void run() {
                for(auto group: callbacks) {
                    for(auto test: callbacks[group]) {
                        try {
                            callbacks[group][test]();
                            // success
                        }
                        catch (...) {
                            // failure
                        }
                    }
                }
            } 

        protected:
            std::map<std::string, std::map<std::string, callback_t>> callbacks;
    }

}

#define EXPECT_TRUE(_condition_) if ((_condition_) != true) { throw std::exception(); }
#define EXPECT_FALSE(_condition_) if ((_condition_) != false) { throw std::exception(); }

#define EXPECT(_condition_) EXPECT_TRUE(_condition_);
#define EXPECT_STREQ(_left_, _right_) EXPECT_TRUE(std::string(_left_).compare(std::string(_right_)) == 0)
#define EXPECT_STRNEQ(_left_, _right_) EXPECT_FALSE(std::string(_left_).compare(std::string(_right_)) == 0)
#define EXPECT_EQ(_left_, _right_) EXPECT_TRUE(_left == _right_)
#define EXPECT_NEQ(_left_, _right_) EXPECT_FALSE(_left == _right_)

#define TEST(_group_, _name_, _callback_) utilities::test::singleton().add(_group_, _name_, _callback_);