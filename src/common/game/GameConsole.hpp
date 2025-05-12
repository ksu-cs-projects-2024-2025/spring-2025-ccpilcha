#pragma once

#include <string>
#include <stdlib.h>
#include <vector>
#include <variant>
#include <sstream>

struct Command;
struct CommandArgument;

struct Command {
    std::string rawCommand; // the entire text

    enum class Type {
        UNKNOWN,
        
        ECHO,
        TELEPORT,
        FUNC,
        GET
    } type;

    std::vector<CommandArgument> args;

    static Type GetType(std::string str)
    {
        if (str == "echo") return Type::ECHO;
        if (str == "tp") return Type::TELEPORT;
        if (str == "func") return Type::FUNC;
        if (str == "get") return Type::GET;
        return Type::UNKNOWN;
    }
};

typedef std::variant<std::string, int, float, Command> CommandToken;

struct CommandArgument {
    std::string rawToken; // just the token text
    CommandToken token;

    enum class Literal {
        // literals
        TEXT,
        INT,
        FLOAT
    } type;

    static Literal InferType(const std::string& token);
};

struct CommandResult {
    int code; // 0 is success -1 error or somehting
    std::stringstream message;
    CommandToken result;

    // Default constructor
    CommandResult() = default;

    // Copy constructor
    CommandResult(const CommandResult& other)
      : code(other.code),
        result(other.result)
    {
        // copy the contents of the stream buffer
        message.str(other.message.str());
    }

    // Copy‐assignment
    CommandResult& operator=(const CommandResult& other) {
        if (this == &other) return *this;
        code   = other.code;
        result = other.result;
        // copy the underlying string too
        message.str(other.message.str());
        return *this;
    }

    // Move operations can stay defaulted
    CommandResult(CommandResult&&) = default;
    CommandResult& operator=(CommandResult&&) = default;

    std::string toString() {
        return std::visit([](const auto& value) -> std::string {
            using T = std::decay_t<decltype(value)>;
    
            if constexpr (std::is_same_v<T, std::string>) {
                return value;
            } else if constexpr (std::is_same_v<T, int>) {
                return std::to_string(value);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::to_string(value);
            } else if constexpr (std::is_same_v<T, Command>) {
                return "<subcommand: " + value.rawCommand + ">";
            } else {
                return "<unknown>";
            }
        }, result);
    }
};


class GameConsole {
    std::vector<std::pair<Command, CommandResult>> history;

    std::vector<std::string> tokenizeCommand(const std::string& input);

public:

    Command ParseCommand(const std::string &input);

    void LogCommand(const Command &command, const CommandResult& result)
    {
        this->history.emplace_back(std::make_pair(command, result));
    }

};