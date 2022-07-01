namespace utilities {

    template <class type, class method>
    class scoped {
    public:
        scoped();
        scoped(type instance, method callback) {
            this->instance = std::make_unique<type>(instance);
            this->callback = std::make_unique<method>(callback);
        }
        scoped(const scoped& ref) {
            *this = ref;
        }

        scoped& operator=(const scoped& rval) {
            this->instance = std::move(rval.instance);
            this->callback = std::move(rval.callback);
            return *this;
        }

        ~scoped() {
            if(instance != NULL && callback != NULL) std::invoke(*callback, *instance);
        }

        std::unique_ptr<type> instance = NULL;
        std::unique_ptr<method> callback = NULL;
    };

    template<typename t>
    std::vector<int> indices(const std::vector<t>& list) {
        std::vector<int> results(list.size());
        int i = 0;
        for(auto & v: list) {
            results[i++] = i;
        }
        return results;
    }

    std::vector<int> range(int elements);
    std::vector<int> range(int start, int end);

    std::string rtrim(std::string in);
    std::string ltrim(std::string in);
    std::string trim(std::string in);

    std::string lc(std::string in);
    std::string uc(std::string in);

    bool numeric(std::string in);

    double clamp(double value, double min, double max);

    std::string extension(const std::string& in);

//    std::string format(std::string str, ...);

    std::string dirname(const std::string& in);

    std::string join(std::string, std::vector<std::string> arguments);
//    std::vector<std::string> split(std::string expression, std::string buffer, int limit = 0);

    std::vector<std::string> tokenize(std::string input, std::string delimiter = "", int tokens=0);

//    std::string substitute(std::string input, std::string expression, std::string replacement, bool global = true);
//    std::string substitute(std::string input, std::vector<std::string>& replacement);

    std::string base(std::string in, int target_ordinal, int current_ordinal = 10);
    std::string base64(std::string in);

    std::string uuid();

    void sleep(time_t milliseconds); // TODO: move this over to std::chrono types

    template<class T> T read(std::istream& input, size_t bytes = sizeof(T)) {
        char buffer[8];
        input.read(buffer, bytes);
        return *reinterpret_cast<T*>(buffer);
    }

    template<> std::string read<std::string>(std::istream& input, size_t bytes);

    class percentage {
    public:
        void set(int current) {
            if (current > 100) {
                current = 100;
            }
            if (current < 0) {
                current = 0;
            }
            std::lock_guard<std::mutex> scoped(lock);
            value = current;
        }

        int get() {
            std::lock_guard<std::mutex> scoped(lock);
            return value;
        }

    private:
        int value;
        std::mutex lock;
    };

    class text {
    public:
        text(int entries = 40) {
            buffer = entries;
        }

        void add(std::string message="") {
            std::lock_guard<std::mutex> scoped(lock);
            for (auto line : utilities::tokenize(message)) {
                data.push_back(line);
                if (data.size() > buffer) {
                    data.pop_front();
                }
            }
        }

        void remove(int count = -1) {
            std::lock_guard<std::mutex> scoped(lock);
            if (count == -1 && data.size()) {
                data.pop_back();
            }
            else {
                while (count-- && data.size() && data.back().empty() == false) {
                    data.back().pop_back();
                }
            }
        }

        void append(std::string message) {
            std::lock_guard<std::mutex> scoped(lock);
            if (data.size()) {
                std::next(data.begin(), index)->append(message);
            }
            else {
                data.push_back(message);
            }
        }

        void truncate(int count) {
            std::lock_guard<std::mutex> scoped(lock);
            if (data.size() == 0) {
                return;
            }
            auto current = std::next(data.begin(), index);
            if (count >= current->size()) {
                current->clear();
            }
            else {
                current->erase(current->size() - count, count);
            }
        }

        std::vector<std::string> get() {
            std::lock_guard<std::mutex> scoped(lock);
            std::vector<std::string> contents;
            int iteration = 0;
            int count = 0;
            for (auto &message: data) {
                if (iteration++ < index) {
                    continue;
                }
                contents.push_back(message);
                if (limit && ++count >= limit) {
                    break;
                }
            }
            return contents;
        }

        int size() {
            return data.size();
        }

        void clear() {
            std::lock_guard<std::mutex> scoped(lock);
            data.clear();
        }

        int position(int amount=0) {
            offset += amount;

            if (offset < 0) {
                offset = 0;
            }
            if (offset >= data.size()) {
                offset = data.size() - 1;
            }

            index = (data.size() - 1) - offset;

            return offset;
        }

        int buffer;

        int index = 0;
        int offset = 0;
        int limit = 0;

    private:
        std::list<std::string> data;
        std::mutex lock;
    };

    // https://philippegroarke.com/posts/2018/chrono_for_humans/

    using time_t = std::chrono::system_clock::time_point;

    using milliseconds_t = std::chrono::duration<double, std::milli>;
    using seconds_t = std::chrono::duration<double>;
    using minutes_t = std::chrono::duration<double, std::ratio<60>>;
    using hours_t = std::chrono::duration<double, std::ratio<3600>>;
    using days_t = std::chrono::duration<double, std::ratio<86400>>;
    using weeks_t = std::chrono::duration<double, std::ratio<604800>>;
    using months_t = std::chrono::duration<double, std::ratio<2629746>>;
    using years_t = std::chrono::duration<double, std::ratio<31556952>>;

}