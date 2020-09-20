#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace argunaught
{

struct Option
{
    std::string longName, shortName;
    std::string description;
    int numParams = 0;
    std::vector<std::string> values;
};

class ParseResult
{
// public:
//     OptionsList globalOptions;
//     CommandList commands;
};

using OptionsList = std::vector<Option>;
using CommandHandler = std::function<int (ParseResult&)>;

struct Command
{
    Command(std::string n, OptionsList opt, CommandHandler f);
    std::string name;
    CommandHandler handler;
    OptionsList options;
    //Command& options(OptionsList options);
};

using CommandList = std::vector<Command>;


class Parser
{
private:
    CommandList mCommands;
    OptionsList mOptions;

public:
    Parser();

    Parser& command(std::string name, CommandHandler func);
    Parser& command(std::string name, OptionsList options, CommandHandler func);
    Parser& options(OptionsList options);

};

}
