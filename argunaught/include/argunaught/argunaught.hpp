#pragma once

#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>

namespace argunaught
{

#ifdef TRACE_OPTIONS
#define ARGUNAUGHT_TRACE(msg, ...) printf(msg, ...)
#else
#define ARGUNAUGHT_TRACE(msg, ...) 
#endif

struct Option
{
    std::string longName, shortName;
    std::string description;
    int maxNumParams = 0;
};

struct OptionResult
{
    std::string optionName;
    std::vector<std::string> values;
};

enum class OptionError
{
    None,
    OptionAlreadyExists
};

class OptionList
{
private:
    std::vector<Option> mOptions;

public:
    OptionList() {}

    OptionList(std::vector<Option> opts);
    OptionError addOption(Option opt);
    std::optional<Option> findShortOption(std::string optionName) const;
    std::optional<Option> findLongOption(std::string optionName) const;

    const std::vector<Option>& values() const {return mOptions; }
};

using OptionResultList = std::vector<OptionResult>;
struct Command;

struct Error {
    int pos;
    std::string value;
};

class ParseResult
{
public:
    // Options found, merged result of global and command options.
    OptionResultList options;

    std::vector<std::string> positionalArgs;

    // Command selected, if any.
    std::shared_ptr<Command> command;

    std::vector<Error> errors;
    bool hasError() const { return errors.size() > 0; }
};

using CommandHandler = std::function<int (ParseResult&)>;

struct Command
{
    Command(std::string n, std::string h, std::vector<Option> opt, CommandHandler f);
    std::string name, help;
    CommandHandler handler;
    OptionList options;
    //Command& options(OptionList options);
};

using CommandList = std::vector<std::shared_ptr<Command>>;

class Parser
{
private:
    std::string mName;
    CommandList mCommands;
    OptionList mOptions;

    std::optional<OptionResult> parseOption(
            std::shared_ptr<Command> command, 
            std::deque<std::string>& parseText,
            ParseResult& parseResult) const;

public:
    Parser(std::string programName);

    Parser& command(std::string name, std::string help, CommandHandler func);
    Parser& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func);
    Parser& options(std::vector<Option> options);

    const CommandList& commands() const { return mCommands; }
    const OptionList& options() const { return mOptions; }

    std::string help() const;
    ParseResult parse(int argc, char* argv[]) const;
};

}
