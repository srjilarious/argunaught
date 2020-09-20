#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

namespace argunaught
{

struct Option
{
    std::string longName, shortName;
    std::string description;
    int numParams = 0;
    std::vector<std::string> values;
};

using OptionsList = std::vector<Option>;
struct Command;

struct Error {
    int pos;
    std::string value;
};

class ParseResult
{
public:
    // Options found, merged result of global and command options.
    OptionsList options;

    // Command selected, if any.
    std::shared_ptr<Command> command;

    std::vector<Error> errors;
    bool hasError() const { return errors.size() > 0; }
//     OptionsList globalOptions;
//     CommandList commands;
};

using CommandHandler = std::function<int (ParseResult&)>;

struct Command
{
    Command(std::string n, OptionsList opt, CommandHandler f);
    std::string name;
    CommandHandler handler;
    OptionsList options;
    //Command& options(OptionsList options);
};

using CommandList = std::vector<std::shared_ptr<Command>>;

class Parser
{
private:
    CommandList mCommands;
    OptionsList mOptions;

    Option parseOption(std::vector<std::string>& parseText);

public:
    Parser();

    Parser& command(std::string name, CommandHandler func);
    Parser& command(std::string name, OptionsList options, CommandHandler func);
    Parser& options(OptionsList options);

    const CommandList& commands() const { return mCommands; }
    const OptionsList& options() const { return mOptions; }

    ParseResult parse(int argc, char* argv[]) const;
};

}
