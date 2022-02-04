#pragma once

#include <string>

#include "forward_decl.hpp"

namespace argunaught
{

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


class Parser;
class Option;

//! Base interface for formatting command and option help.
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

//! Default help formatter using ANSI color sequences if stdout is a tty.
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

    // A flag for whether we should print out ANSI colors
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

}