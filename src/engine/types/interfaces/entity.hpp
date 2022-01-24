#pragma once

namespace type {

    class entity : virtual public type::info {
    public:
        class animation {
        public:
            int frame = 0;
            utilities::time_t start;
            utilities::seconds_t elapsed;

            std::vector<utilities::seconds_t> duration;
            std::vector<type::object> frames;
        };
        std::string state;
        std::map<std::string, animation> animations;

        std::list<spatial::position> path;

    public:
        entity() {}

        bool empty() {
            return true;
        }
    };

}
