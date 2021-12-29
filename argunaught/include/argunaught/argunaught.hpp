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
#define ARGUNAUGHT_TRACE(...) printf(__VA_ARGS__)
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
    OptionList(const OptionList& opts);
    OptionError addOption(Option opt);
    OptionError addOptions(const OptionList& opts);
    std::optional<Option> findShortOption(std::string optionName) const;
    std::optional<Option> findLongOption(std::string optionName) const;

    const std::vector<Option>& values() const {return mOptions; }
};

using OptionResultList = std::vector<OptionResult>;
struct Command;

enum class ParseErrorType
{
    UnknownOption,
};

struct ParseError {
    ParseErrorType type;
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

    std::vector<ParseError> errors;

    // Used with commands that are marked as handling sub commands.
    // This allows the rest of the arguments to be parsed with a 
    // sub parser.
    struct SubCommandInfo
    {
        std::deque<std::string> remainingArgs;

        // List of previous Parser global and selected command options.
        // This allows sub commands to inherit options from a previous
        // parser while adding new options in their sub parser.
        OptionList prevOptions;
    } subCommandInfo;

    bool hasError() const { return errors.size() > 0; }

    // Returns whether a command was found.
    bool hasCommand() const { return command != nullptr; }
    
    // Returns the option result and its params if it was found during parsing.
    std::optional<OptionResult> getOption(std::string optionLongName) const;

    // Returns whether the given option was found during parsing.
    bool hasOption(std::string optionLongName) const;

    // Runs the sub command if there is one returning its value or
    // if there is no sub-command returns -1.
    int runCommand() const;
};

using CommandHandler = std::function<int (const ParseResult&)>;

struct Command
{
    Command(std::string n, std::string h, std::vector<Option> opt, CommandHandler f, bool _handlesSubCommands);
    std::string name, help;
    CommandHandler handler;
    OptionList options;
    bool handlesSubCommands;
};

using CommandPtr = std::shared_ptr<Command>;
using CommandList = std::vector<CommandPtr>;

class Parser
{
private:
    std::string mName;
    std::string mBanner;
    CommandList mCommands;
    OptionList mOptions;

    std::optional<OptionResult> parseOption(
            std::shared_ptr<Command> command, 
            std::deque<std::string>& parseText,
            ParseResult& parseResult) const;

    std::string optionHelpName(Option const& opt) const;
    std::string replaceAll(std::string const& orig, char c, std::string const& replace) const;

public:
    Parser(std::string programName, std::string banner = "");

    Parser& command(std::string name, std::string help, CommandHandler func, bool handlesSubCommands = false);
    Parser& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func, bool handlesSubCommands = false);
    Parser& options(std::vector<Option> options);
    Parser& options(OptionList options);

    const CommandList& commands() const { return mCommands; }
    const OptionList& options() const { return mOptions; }

    // Creates a string for help, 
    std::string help(std::size_t minJustified = 0, std::size_t maxJustified = 20) const;
    ParseResult parse(int argc, const char* argv[], OptionResultList existingOptions = {}) const;
};

}
