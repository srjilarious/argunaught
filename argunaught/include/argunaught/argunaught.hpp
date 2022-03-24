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

//! An enum of errors checked for during parser configuration.
enum class ParserConfigErrorType
{
    NoError,
    LongOptionNameMissing,
    DuplicateOption,
    CommandNameMissing,
    DuplicateCommandName,
    CommandGroupNameMissing,
};

//! A structure for capturing information about parser configuration errors.
struct ParserConfigError
{
    //! The type of parser configuration error found
    ParserConfigErrorType type;

    //! Information about the configuration error
    std::string message;
};

//! An enum of errors checked for during parsing.
enum class ParseErrorType
{
    UnknownOption,
};

//! Information about errors caught while parsing the command line with the
//! current parser.
struct ParseError {
    //! The type of error the parser found
    ParseErrorType type;

    //! Which item in the list of command line tokens generated the error
    int pos;

    //! The problematic command line token.
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
    //! The 'long' option name following `--`.
    std::string longName;

    //! The 'short' option name following `-`.
    std::string shortName;

    //! A description of the option, used for generating help.
    //! `\n` is replaced in the default help formatting with proper indentation.
    std::string description;

    //! The number of parameters this option expects, 
    //
    //! `-1` means an unlimited number of parameters are accepted, with the 
    //! parser stopping if an option (starting with `-` or `--`) is found.
    //! Similarly a lone `--` will end option parameter parsing and move to 
    //! positional parameters.
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

    //! Adds an option to the list of supported options
    ParserConfigErrorType addOption(Option opt);

    //! Adds an existing option list to this one
    //
    //! Note: No duplicates are checked for, so any existing options 
    //!       in this list will end up taking precedence.
    ParserConfigErrorType addOptions(const OptionList& opts);

    //! Looks for an option based on its short name
    std::optional<Option> findShortOption(std::string optionName) const;

    //! Looks for an option using its long name
    std::optional<Option> findLongOption(std::string optionName) const;

    //! Returns a constant reference to all of the contained options.
    const std::vector<Option>& values() const { return mOptions; }
};


//! The result of parsing a command line for commands, options and associated 
//! parameters.
class ParseResult
{
    friend class Parser;

private:
    //! We keep track of the options from the parser for use in sub parsing.
    OptionList optionsList;

    //! We track the current item position in case of errors.
    std::size_t currItemPos = 0;
    
public:
    //! Options found, merged result of global and command options.
    OptionResultList options;

    //! Positional arguments found after parsing any command and options with 
    //! their parameters.
    std::vector<std::string> positionalArgs;

    //! Command selected, if any.
    CommandPtr command;
    
    //! List of any errors found during parsing
    std::vector<ParseError> errors;

    //! Helper method to check if an error was found during parsing.
    bool hasError() const { return errors.size() > 0; }

    //! Returns whether a command was found.
    bool hasCommand() const { return command != nullptr; }
    
    //! Returns the option result and its params if it was found during parsing.
    std::optional<OptionResult> getOption(std::string optionLongName) const;

    //! Returns whether the given option was found during parsing.
    bool hasOption(std::string optionLongName) const;

    //! Runs the sub command if there is one returning its value or
    //! if there is no sub-command returns -1  automatically.
    int runCommand() const;
};

//! Definition of a sub command that contains its own functor for execution.
struct Command 
{
    Command(std::string n, std::string h, std::vector<Option> opt, CommandHandler f);

    //! The name of the command, i.e. the command line token used to invoke this command.
    std::string name;
    
    //! A description of the command for help text.
    //! `\n` is replaced in the default help formatting with proper indentation.
    std::string description;

    //! A list of options that are specific to this command.
    OptionList options;

    //! The function to call when running this command.  Returns an int to allow returning
    //! the result from main when running the command.
    CommandHandler handler;
};

//! A special command that actually creates its own parser with its own commands/options.
struct SubParser 
{
    SubParser(std::string n, std::string h, std::vector<Option> opt, SubParserHandler f);

    //! The name of the sub parser.  Acts like a command that specifically allows its own
    //! sub parsing call.
    std::string name;

    //! A description of the sub parsing special command.
    //! `\n` is replaced in the default help formatting with proper indentation.
    std::string description;

    //! A list of options that are specific to this parser.
    OptionList options;

    //! A handling function that can define and run its own parser on the remaining 
    //! command line tokens.
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

    //! The name of the group of commands, used for grouping commands in help text. 
    std::string name;

    //! A description of the group of commands.
    std::string description;

    //! A list of commands in this group
    CommandList commands;

    //! A list of subparsers (which are just special commands)
    SubParserList subParsers;

    //! Creates a command in the group
    CommandGroup& command(std::string name, std::string help, CommandHandler func);

    //! Creates a command in the group, with extra options for the command
    CommandGroup& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func);

    //! Creates a subparser in the group
    CommandGroup& subParser(std::string name, std::string help, SubParserHandler func);

    //! Creates a subparser in the group, with extra options for the subparser
    CommandGroup& subParser(std::string name, std::string help, std::vector<Option> options, SubParserHandler func);

    //! Ends the group returning a refernce to the parent instantiating parent to allow a fluent interface.
    Parser& endGroup() { return *mParent; }
};


//! The main parser class that contains sub-parsers, commands, and options to 
//! aid in parsing a command line.
class Parser
{
    friend class HelpFormatter;
    friend class DefaultHelpFormatter;

private:
    //! Name of the program
    std::string mName;

    //! A special banner to use instead of the program name in help text generation.
    std::string mBanner;

    //! A program description printed after the name/banner
    std::string mDescription;
    
    //! Usage text to show in help after the description
    std::string mUsage;

    //! A list of commands that can be parsed
    CommandList mCommands;

    //! A list of subparsers (which are special commands with their own parsers)
    SubParserList mSubParsers;

    //! Global options for the program
    OptionList mOptions;

    //! Any grouped commands
    std::vector<CommandGroup> mGroups;

    //! Any parser configuration errors found
    std::vector<ParserConfigError> mConfigErrors;

    //! Helper method to parse an option, handling potentially command specific options
    //! modifying the deque of command line tokens and adding results to the parseResult.
    std::optional<OptionResult> parseOption(
            std::shared_ptr<Command> command, 
            std::deque<std::string>& parseText,
            ParseResult& parseResult) const;

    //! Checks if a command or subparser name has already been registered with the parser.
    bool checkCommandNameExists(std::string name) const;

public:
    Parser(std::string programName, std::string banner = "");

    //! Sets the parser's description text.
    Parser& description(std::string d);
    
    //! Sets the parer's usage text.
    Parser& usage(std::string u);

    //! Creates a command in the parser.
    Parser& command(std::string name, std::string help, CommandHandler func);

    //! Creates a command in the parser with command specific options.
    Parser& command(std::string name, std::string help, std::vector<Option> options, CommandHandler func);
    
    //! Creates a subparser (a special type of command) in the parser.
    Parser& subParser(std::string name, std::string help, SubParserHandler func);

    //! Creates a  subparser (a special type of command) in the parser with command specific options.
    Parser& subParser(std::string name, std::string help, std::vector<Option> options, SubParserHandler func);
    
    //! Adds a list of options to the parser
    Parser& options(std::vector<Option> options);

    //! Adds a list of options to the parser
    Parser& options(const OptionList& options);

    //! Creates a new command group that can have commands or subparsers added to create 
    //! a logical grouping of commands for the program.  Useful for the generation of the help.
    CommandGroup& group(std::string name);

    //! Creates a new command group that can have commands or subparsers added to create 
    //! a logical grouping of commands for the program.  Useful for the generation of the help.
    CommandGroup& group(std::string name, std::string description);

    //! Returns a const reference to the command list defined for this parser.
    const CommandList& commands() const { return mCommands; }

    //! Returns a const reference to the list of global options defined on this parser.
    const OptionList& options() const { return mOptions; }

    //! Looks for a command defined of the parser and returns it, or nullptr if not found.
    CommandPtr getCommand(std::string name);

    //! Parses the command line, given argc and argv from main.
    ParseResult parse(int argc, const char* argv[]) const;

    //! Parses the given arguments, assumes the executable name has been skipped.
    ParseResult parse(std::deque<std::string> args, OptionResultList existingOptions = {}) const;

    //! Allows performing sub command parsing using options from previous 
    //! parser call.
    ParseResult parse(ParseResult const& prevParseResult);

    //! Returns if a parser configuration error was found.
    bool hasConfigurationError() const { return mConfigErrors.size() > 0; }

    //! Returns the list of parser configuration errors found, if any.
    std::vector<ParserConfigError> parserConfigErrors() const { return mConfigErrors; }
};

}
