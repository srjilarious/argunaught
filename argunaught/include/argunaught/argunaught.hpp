#pragma once

#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <memory>
#include <optional>

#include "forward_decl.hpp"
#include "formatting.hpp"

namespace argunaught
{




#ifdef TRACE_OPTIONS
#define ARGUNAUGHT_TRACE(...) printf(__VA_ARGS__)
#else
#define ARGUNAUGHT_TRACE(msg, ...) 
#endif

//! An enum of errors checked for during parsing.
enum class ParseErrorType
{
    UnknownOption,
};

//! An enum of errors checked for during option parsing
enum class OptionError
{
    None,
    OptionAlreadyExists
};

//! Information about errors caught while parsing the command line with the
//! current parser.
struct ParseError {
    ParseErrorType type;
    int pos;
    std::string value;
};

//! Command line option descritor
/*!
 *  Describes the short and long names for an option.  Contains a description 
 *  of what the option is used for and declares how many parameters if any the 
 *  option takes.  Use `-1` to allow infinite params, broken by another option 
 *  or `--` to start positional parameters.
 */
struct Option
{
    std::string longName, shortName;
    std::string description;
    int maxNumParams = 0;
};

//! An instance of an option found during parsing and any parameters associated.
struct OptionResult
{
    std::string optionName;
    std::vector<std::string> values;
};

//! A collection of options contained in the parser.
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


//! The result of parsing a command line for commands, options and associated 
//! parameters.
class ParseResult
{
    friend class Parser;

private:
    // We keep track of the options from the parser for use in sub parsing.
    OptionList optionsList;

public:
    // Options found, merged result of global and command options.
    OptionResultList options;

    std::vector<std::string> positionalArgs;

    // Command selected, if any.
    std::shared_ptr<Command> command;

    std::vector<ParseError> errors;

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

//! Definition of a sub command that contains its own functor for execution.
struct Command 
{
    Command(std::string n, std::string h, std::vector<Option> opt, CommandHandler f);
    std::string name, help;
    OptionList options;
    CommandHandler handler;
};

//! A special command that actually creates its own parser with its own commands/options.
struct SubParser 
{
    SubParser(std::string n, std::string h, std::vector<Option> opt, SubParserHandler f);
    std::string name, help;
    OptionList options;
    SubParserHandler handler;
};

//! A way to group commands that are related semantically.
class CommandGroup
{
private:
    Parser* mParent;

public:
    CommandGroup() = default;
    CommandGroup(Parser* parent) : mParent(parent) {}
    CommandGroup(Parser* parent, std::string _name, std::string _desc = "");  
    std::string name;
    std::string description;
    CommandList commands;
    SubParserList subParsers;

    CommandGroup& command(std::string name, std::string help, CommandHandler func);
    CommandGroup& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func);

    CommandGroup& subParser(std::string name, std::string help, SubParserHandler func);
    CommandGroup& subParser(std::string name, std::string help, std::vector<Option> options, SubParserHandler func);

    Parser& endGroup() { return *mParent; }
};

//! The main parser class that contains sub-parsers, commands, and options to 
//! aid in parsing a command line.
class Parser
{
    friend class HelpFormatter;
    friend class DefaultHelpFormatter;

private:
    std::string mName;
    std::string mBanner;
    CommandList mCommands;
    SubParserList mSubParsers;
    OptionList mOptions;
    Parser* mParent = nullptr;

    // Any grouped commands (which also will be added to commands)
    std::vector<CommandGroup> mGroups;

    std::optional<OptionResult> parseOption(
            std::shared_ptr<Command> command, 
            std::deque<std::string>& parseText,
            ParseResult& parseResult) const;

public:
    Parser(std::string programName, std::string banner = "");

    Parser& command(std::string name, std::string help, CommandHandler func);
    Parser& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func);
    
    Parser& subParser(std::string name, std::string help, SubParserHandler func);
    Parser& subParser(std::string name, std::string help, std::vector<Option> options, SubParserHandler func);
    
    Parser& options(std::vector<Option> options);
    Parser& options(const OptionList& options);

    const CommandList& commands() const { return mCommands; }
    const OptionList& options() const { return mOptions; }

    CommandGroup& group(std::string name);

    ParseResult parse(int argc, const char* argv[]) const;
    
    // Parses the given arguments, assumes the executable name has been skipped.
    ParseResult parse(std::deque<std::string> args, OptionResultList existingOptions = {}) const;

    // Allows performing sub command parsing using options from previous 
    // parser call.
    ParseResult parse(ParseResult const& prevParseResult);
};

}
