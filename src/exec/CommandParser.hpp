#ifndef WASMVM_EXEC_CommandParser_DEF
#define WASMVM_EXEC_CommandParser_DEF

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <utility>
#include <initializer_list>
#include <filesystem>

struct CommandParser {

    using Arg = std::variant<std::monostate, std::string, std::vector<std::string>>;

    struct Fixed {
        std::string name;
        std::string desc = "";
        unsigned number = 0;
    };
    struct Optional : public Fixed {
        std::string alias = "";
    };

    CommandParser(int argc, const char* argv[],
        std::initializer_list<std::variant<Fixed, Optional>> options,
        std::string desc = ""
    );

    std::optional<Arg> operator[](const std::string key);

    std::filesystem::path program;

private:
    std::unordered_map<std::string, Arg> args;
};

#endif