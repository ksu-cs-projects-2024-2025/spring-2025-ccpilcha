#include "GameConsole.hpp"

std::vector<std::string> GameConsole::tokenizeCommand(const std::string &input)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false, escape = false;

    for (char c : input) {
        if (escape) {
            current += c;
            escape = false;
        }
        else if (c == '\\') {
            escape = true;
        }
        else if (c == '"') {
            inQuotes = !inQuotes;
        }
        else if (std::isspace(c) && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        }
        else {
            current += c;
        }
    }
    if (!current.empty())
        tokens.push_back(current);
    return tokens;
}

Command GameConsole::ParseCommand(const std::string &input)
{
    Command cmd = Command();
    cmd.rawCommand = input;

    std::vector<std::string> tokens = tokenizeCommand(input);
    std::string cmdToken = tokens[0];

    cmd.type = Command::GetType(cmdToken);

    // now we need to focus on the arguments
    tokens.erase(tokens.begin());
    for (auto& token : tokens)
    {
        CommandArgument arg;
        arg.rawToken = token;
        arg.type = CommandArgument::InferType(token);
        switch (arg.type) {
            case CommandArgument::Literal::INT:
                arg.token = std::stoi(token);
                break;
            case CommandArgument::Literal::FLOAT:
                arg.token = std::stof(token);
                break;
            default:
                arg.token = token;
                break;
        }
        cmd.args.emplace_back(arg);
    }

    return cmd;   
}

CommandArgument::Literal CommandArgument::InferType(const std::string& token)
{
    if (token.empty()) return Literal::TEXT;

    // Binary
    if (token.size() > 2 && token[0] == '0' && (token[1] == 'b' || token[1] == 'B')) {
        for (char c : token.substr(2)) {
            if (c != '0' && c != '1') return Literal::TEXT;
        }
        return Literal::INT;
    }

    // Hex
    if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X')) {
        for (char c : token.substr(2)) {
            if (!std::isxdigit(c)) return Literal::TEXT;
        }
        return Literal::INT;
    }

    // Float / Scientific
    if (token.find('.') != std::string::npos || token.find('e') != std::string::npos || token.find('E') != std::string::npos) {
        try {
            std::stof(token);
            return Literal::FLOAT;
        } catch (...) {}
    }

    // Int
    try {
        std::stoi(token); // throws if invalid
        return Literal::INT;
    } catch (...) {}

    return Literal::TEXT;
}
