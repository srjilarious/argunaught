#pragma once

#include <string>

#include "forward_decl.hpp"

namespace argunaught
{

namespace color
{
    constexpr const char ResetColor[]       = "\033[0m";

namespace foreground
{
    constexpr const char BlackColor[]       = "\033[30m";
    constexpr const char RedColor[]         = "\033[31m";
    constexpr const char GreenColor[]       = "\033[32m";
    constexpr const char YellowColor[]      = "\033[33m";
    constexpr const char BlueColor[]        = "\033[34m";
    constexpr const char MagentaColor[]     = "\033[35m";
    constexpr const char CyanColor[]        = "\033[36m";
    constexpr const char WhiteColor[]       = "\033[37m";
    
    constexpr const char DimBlackColor[]    = "\033[2;30m";
    constexpr const char DimRedColor[]      = "\033[2;31m";
    constexpr const char DimGreenColor[]    = "\033[2;32m";
    constexpr const char DimYellowColor[]   = "\033[2;33m";
    constexpr const char DimBlueColor[]     = "\033[2;34m";
    constexpr const char DimMagentaColor[]  = "\033[2;35m";
    constexpr const char DimCyanColor[]     = "\033[2;36m";
    constexpr const char DimWhiteColor[]    = "\033[2;37m";

    constexpr const char ItalicColorMode[]  = "\033[3m";
    constexpr const char UnderlineColorMode[]  = "\033[4m";

    constexpr const char GrayColor[]        = "\033[1;90m";
    constexpr const char BoldRedColor[]     = "\033[1;91m";
    constexpr const char BoldGreenColor[]   = "\033[1;92m";
    constexpr const char BoldYellowColor[]  = "\033[1;93m";
    constexpr const char BoldBlueColor[]    = "\033[1;94m";
    constexpr const char BoldMagentaColor[] = "\033[1;95m";
    constexpr const char BoldCyanColor[]    = "\033[1;96m";
    constexpr const char BoldWhiteColor[]   = "\033[1;97m";

    // std::string color256(uint8_t which) {
    //     return std::string("\u001b[38;5;") + std::to_string(which) + "m";
    // }
} // foreground

namespace background
{
    constexpr const char BlackColor[]       = "\033[40m";
    constexpr const char RedColor[]         = "\033[41m";
    constexpr const char GreenColor[]       = "\033[42m";
    constexpr const char YellowColor[]      = "\033[43m";
    constexpr const char BlueColor[]        = "\033[44m";
    constexpr const char MagentaColor[]     = "\033[45m";
    constexpr const char CyanColor[]        = "\033[46m";
    constexpr const char WhiteColor[]       = "\033[47m";

    constexpr const char GrayColor[]        = "\033[40;1m";
    constexpr const char BoldRedColor[]     = "\033[41;1m";
    constexpr const char BoldGreenColor[]   = "\033[42;1m";
    constexpr const char BoldYellowColor[]  = "\033[43;1m";
    constexpr const char BoldBlueColor[]    = "\033[44;1m";
    constexpr const char BoldMagentaColor[] = "\033[45;1m";
    constexpr const char BoldCyanColor[]    = "\033[46;1m";
    constexpr const char BoldWhiteColor[]   = "\033[47;1m";

    // std::string color256(uint8_t which) {
    //     return std::string("\u001b[48;5;") + std::to_string(which) + "m";
    // }
} // background
} // color


//! A helper method that handles word wrapping and formatting like embedded `\n`s.
std::size_t 
formatAndAppendText(
        std::string& dest, 
        std::size_t currLineLen,
        std::size_t currIndentAmount,
        std::size_t maxLineLength,
        std::string value);

class Parser;
class Option;

//! Base interface for formatting command and option help.
class HelpFormatter 
{
protected:
    std::string mHelpString;
    std::size_t mCurrLineLength = 0;
    std::size_t mCurrIndentAmount = 0;

    std::size_t mMaxLineWidth = 1024;

    virtual void appendText(std::string valuem, bool handleFormatting=false);
    virtual void newLine();
    virtual void indent(std::size_t amount);

    //! Finds the longest option/command name
    std::size_t findMaxOptComLength(Parser& parser, std::size_t indentPerLevel);

    virtual void optionHelpName(Option const& opt);
    virtual std::size_t optionHelpNameLength(Option const& opt);

    virtual void programName(std::string name) = 0;

    virtual void beginGroup(std::string value) = 0;
    virtual void endGroup() = 0;

    virtual void commandName(std::string key) = 0;
    virtual void optionName(std::string key) = 0;
    virtual void optionDash(bool longDash) = 0;
    virtual void optionSeperator() = 0;

    virtual void seperator() = 0;

    virtual void groupDescription(std::string value) = 0;
    virtual void commandDescription(std::string value) = 0;
    virtual void optionDescription(std::string value) = 0;

    virtual void programDescription(std::string value) = 0;
    virtual void programUsage(std::string value) = 0;

public:
    virtual std::string helpString() = 0;;
};

//! Styling options for the default help formatter implementation.
struct DefaultFormatStyle
{
    std::size_t initialIndentLevel = 4;
    std::size_t spacesPerIndentLevel = 2;
    std::size_t minJustified = 10;
    std::size_t maxJustified = 20;
    
    //! The max line length to be used, actual max line length can be shorter
    //! if the display width is smaller.
    std::size_t maxLineLength = 120;

    std::string programNameColor = color::foreground::BoldGreenColor;

    std::string groupNameColor = std::string(color::foreground::UnderlineColorMode) + color::foreground::BlueColor;
    std::string groupNameSuffix = ":";

    std::string commandNameColor = color::foreground::YellowColor;
    std::string optionNameColor = color::foreground::CyanColor;
    std::string optionDashColor = color::foreground::DimCyanColor;

    std::string optionSeparatorColor = color::foreground::DimCyanColor;
    std::string optionSeparator = ", ";

    std::string separatorColor = color::foreground::DimWhiteColor;
    std::string separator = " - ";

    std::string programDescriptionColor = std::string(color::foreground::ItalicColorMode) + color::foreground::WhiteColor;
    std::string programUsageColor = color::foreground::BoldWhiteColor;
    
    std::string groupDescriptionColor = std::string(color::foreground::ItalicColorMode) + color::foreground::WhiteColor;
    std::string commandDescriptionColor = color::foreground::WhiteColor;
    std::string optionDescriptionColor = color::foreground::WhiteColor;
};

//! Default help formatter using ANSI color sequences if stdout is a tty.
class DefaultHelpFormatter : public HelpFormatter
{
protected:    
    std::size_t mMaxOptComLength = 0;

    Parser& mParser;
    DefaultFormatStyle mStyle;

    // A flag for whether we should print out ANSI colors
    bool mIsTTY = true;

    virtual void generateCommandHelp(CommandPtr com, int maxOptComLength);
    virtual void generateSubParserHelp(SubParserPtr com, int maxOptComLength);

    void resetColor();

public:
    DefaultHelpFormatter(Parser& parser, DefaultFormatStyle style = {}, bool forceNoColor=false);

    void programName(std::string name) override;

    void beginGroup(std::string value) override;
    void endGroup() override;

    void commandName(std::string key) override;
    void optionName(std::string key) override;
    void optionDash(bool longDash) override;
    void optionSeperator() override;

    void seperator() override;

    void groupDescription(std::string value) override;
    void commandDescription(std::string value) override;
    void optionDescription(std::string value) override;

    void programDescription(std::string value) override;
    void programUsage(std::string value) override;

    std::string helpString() override;
};

}