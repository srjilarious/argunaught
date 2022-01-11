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

namespace color
{
    constexpr const char ResetColor[]   = "\033[0m";

namespace foreground
{
    constexpr const char BlackColor[]     = "\033[30m";
    constexpr const char RedColor[]     = "\033[31m";
    constexpr const char GreenColor[]   = "\033[32m";
    constexpr const char YellowColor[]  = "\033[33m";
    constexpr const char BlueColor[]    = "\033[34m";
    constexpr const char MagentaColor[] = "\033[35m";
    constexpr const char CyanColor[]    = "\033[36m";
    constexpr const char WhiteColor[]   = "\033[37m";

    constexpr const char GrayColor[]       = "\033[90m";
    constexpr const char BoldRedColor[]    = "\033[91m";
    constexpr const char BoldGreenColor[]  = "\033[92m";
    constexpr const char BoldYellowColor[] = "\033[93m";
    constexpr const char BoldBlueColor[]   = "\033[94m";
    constexpr const char BoldMagentaColor[]= "\033[95m";
    constexpr const char BoldCyanColor[]   = "\033[96m";
    constexpr const char BoldWhiteColor[]  = "\033[97m";

    // std::string color256(uint8_t which) {
    //     return std::string("\u001b[38;5;") + std::to_string(which) + "m";
    // }
} // foreground

namespace background
{
    constexpr const char BlackColor[]     = "\033[40m";
    constexpr const char RedColor[]     = "\033[41m";
    constexpr const char GreenColor[]   = "\033[42m";
    constexpr const char YellowColor[]  = "\033[43m";
    constexpr const char BlueColor[]    = "\033[44m";
    constexpr const char MagentaColor[] = "\033[45m";
    constexpr const char CyanColor[]    = "\033[46m";
    constexpr const char WhiteColor[]   = "\033[47m";

    constexpr const char GrayColor[]       = "\033[40;1m";
    constexpr const char BoldRedColor[]    = "\033[41;1m";
    constexpr const char BoldGreenColor[]  = "\033[42;1m";
    constexpr const char BoldYellowColor[] = "\033[43;1m";
    constexpr const char BoldBlueColor[]   = "\033[44;1m";
    constexpr const char BoldMagentaColor[]= "\033[45;1m";
    constexpr const char BoldCyanColor[]   = "\033[46;1m";
    constexpr const char BoldWhiteColor[]  = "\033[47;1m";

    // std::string color256(uint8_t which) {
    //     return std::string("\u001b[48;5;") + std::to_string(which) + "m";
    // }
} // background
} // color


enum class ParseErrorType
{
    UnknownOption,
};

enum class OptionError
{
    None,
    OptionAlreadyExists
};

struct ParseError {
    ParseErrorType type;
    int pos;
    std::string value;
};

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
struct SubParser;
class ParseResult;
class Parser;

// A standard command that runs like a miniature main
using CommandHandler = std::function<int (const ParseResult&)>;

// A command that is expected to instantiate a parser in its handler to 
// parse the remaining positional arguments to allow breaking sets of commands into 
// sub groups.
using SubParserHandler = std::function<ParseResult (const Parser& parent, OptionResultList foundOptions, std::deque<std::string> args)>;

using CommandPtr = std::shared_ptr<Command>;
using CommandList = std::vector<CommandPtr>;
using SubParserPtr = std::shared_ptr<SubParser>;
using SubParserList = std::vector<SubParserPtr>;

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

struct Command 
{
    Command(std::string n, std::string h, std::vector<Option> opt, CommandHandler f);
    std::string name, help;
    OptionList options;
    CommandHandler handler;
};

struct SubParser 
{
    SubParser(std::string n, std::string h, std::vector<Option> opt, SubParserHandler f);
    std::string name, help;
    OptionList options;
    SubParserHandler handler;
};

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


class HelpFormatter 
{
protected:
    std::string mHelpString;

    std::size_t findMaxOptComLength(Parser& parser);

    virtual void optionHelpName(Option const& opt);
    virtual size_t optionHelpNameLength(Option const& opt);

    virtual void programName(std::string name) = 0;

    virtual void beginGroup(std::string value) = 0;
    virtual void endGroup() = 0;

    virtual void commandName(std::string key) = 0;
    virtual void optionName(std::string key) = 0;
    virtual void optionDash(bool longDash) = 0;
    virtual void optionSeperator() = 0;

    virtual void seperator() = 0;

    virtual void commandDescription(std::string value) = 0;
    virtual void optionDescription(std::string value) = 0;

public:
    virtual std::string helpString() = 0;;
};

class DefaultHelpFormatter : public HelpFormatter
{
private:
    int initialIndentLevel = 4;
    std::string initialIndent = std::string(initialIndentLevel, ' ');

    int spacesPerIndentLevel = 2;
    int minJustified = 10;
    int maxJustified = 20;
    std::string keyValSep = " - ";
    int keValSepLength = 3;

    std::size_t mMaxOptComLength = 0;


    Parser& mParser;

    // A flag for whether we should print out ANSI colora
    bool mIsTTY = true;

protected: 
    virtual void generateCommandHelp(CommandPtr com, int maxOptComLength);
    virtual void generateSubParserHelp(SubParserPtr com, int maxOptComLength);

public:
    DefaultHelpFormatter(Parser& parser);

    void programName(std::string name) override;

    void beginGroup(std::string value) override;
    void endGroup() override;

    void commandName(std::string key) override;
    void optionName(std::string key) override;
    void optionDash(bool longDash) override;
    void optionSeperator() override;

    void seperator() override;

    void commandDescription(std::string value) override;
    void optionDescription(std::string value) override;

    std::string helpString() override;
};

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
