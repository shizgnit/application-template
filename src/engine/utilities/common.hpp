namespace utilities {

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
}