namespace utilities {

    template <class type, class method>
    class scoped {
    public:
        //typedef void(*callback)();
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

//    std::string format(std::string str, ...);

    std::string dirname(const std::string& in);

    std::string join(std::string, std::vector<std::string> arguments);
//    std::vector<std::string> split(std::string expression, std::string buffer, int limit = 0);

    std::vector<std::string> tokenize(std::string input, std::string delimiter = "");

//    std::string substitute(std::string input, std::string expression, std::string replacement, bool global = true);
//    std::string substitute(std::string input, std::vector<std::string>& replacement);

    std::string base(std::string in, int target_ordinal, int current_ordinal = 10);
    std::string base64(std::string in);

    std::string uuid();

    template<class T> T read(std::istream& input, size_t bytes = sizeof(T)) {
        char buffer[8];
        input.read(buffer, bytes);
        return *reinterpret_cast<T*>(buffer);
    }

    template<> std::string read<std::string>(std::istream& input, size_t bytes);

    class text {
    public:
        text(int entries = 40) {
            limit = entries;
        }

        void add(std::string message) {
            lock.lock();
            data.push_back(message);
            if (data.size() > limit) {
                data.pop_front();
            }
            lock.unlock();
        }

        void append(std::string message) {
            lock.lock();
            if (data.size()) data.back().append(message);
            else data.push_back(message);
            lock.unlock();
        }

        void remove(int count=-1) {
            lock.lock();
            if (count == -1 && data.size()) data.pop_back();
            else while(count-- && data.size() && data.back().empty() == false) data.back().pop_back();
            lock.unlock();
        }

        std::vector<std::string> get() {
            std::vector<std::string> list;
            lock.lock();
            for (auto message : data) {
                list.push_back(message);
            }
            lock.unlock();
            return list;
        }

        void clear() {
            lock.lock();
            data.clear();
            lock.unlock();
        }

        int limit;
    private:

        std::list<std::string> data;
        std::mutex lock;
    };


}