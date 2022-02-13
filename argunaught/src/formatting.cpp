#include <unistd.h> // for STDOUT_FILENO and isatty
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ

#include <argunaught/argunaught.hpp>

namespace 
{

std::string 
replaceAll(std::string const& orig, char c, std::string const& replace)
{
    std::string newStr;
    for(char ch : orig) {
        if(ch == c) {
            newStr += replace;
        }
        else {
            newStr += ch;
        }
    }
    return newStr;
}

std::vector<std::string>
split(std::string const& orig, char c)
{
    std::vector<std::string> result;
    std::string newStr;
    for(char ch : orig) {
        if(ch != c) {
            newStr += ch;
        }
        else {
            result.push_back(newStr);
            newStr = "";
        }
    }
    
    if(newStr.size() != 0) {
        result.push_back(newStr);
    }

    return result;
}

std::size_t
displayWidth()
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return static_cast<std::size_t>(size.ws_col);
}

} // End anonymous namespace

namespace argunaught
{

std::size_t 
formatAndAppendText(
        std::string& dest, 
        std::size_t currLineLen,
        std::size_t currIndentAmount,
        std::size_t maxLineLength,
        std::string value)
{

    // TODO: Handle max allowed line < curr indent amount
    // TODO: Handle hyphenating

    std::size_t start = 0;
    std::size_t prevWordLoc = 0;

    for(std::size_t ii = 0; ii < value.size(); ii++) 
    {
        if(value[ii] == ' ') {
            prevWordLoc = ii;
        }
        else if(value[ii] == '\n') {
            dest += value.substr(start, ii - start) + "\n";
            dest += std::string(currIndentAmount, ' ');
            currLineLen = currIndentAmount;
            start = ii+1;
            ii++; // Jump past the new line.
        }

        if((currLineLen + ii - start) >= maxLineLength) {
            if(start >= prevWordLoc) {
                // hyphenate here.
                dest += value.substr(start, ii - 1 - start) + "-\n";
                dest += std::string(currIndentAmount, ' ');
                currLineLen = currIndentAmount;
                start = ii-1;
                prevWordLoc = start;
            }
            else {
                // word wrap break
                dest += value.substr(start, prevWordLoc - start) + "\n";
                dest += std::string(currIndentAmount, ' ');
                currLineLen = currIndentAmount;
                start = prevWordLoc+1;
            }
        } 
    }
    
    // Write any remaining text.
    std::size_t left = value.size()-start;
    if((currLineLen + left) >= maxLineLength) {
        // if(start == prevWordLoc) {
        //     // hyphenate here.
        // }

        // word wrap break
        dest += value.substr(start, prevWordLoc - start) + "\n";
        dest += std::string(currIndentAmount, ' ');
        auto lastAmount = value.size()-prevWordLoc-1;
        dest += value.substr(prevWordLoc+1, lastAmount);
        currLineLen = currIndentAmount + lastAmount;
    } 
    else {
        dest += value.substr(start, left);
        currLineLen += left;
    }

    return currLineLen;
}

void
HelpFormatter::optionHelpName(Option const& opt)
{
    optionDash(true);
    optionName(opt.longName);

    if(opt.shortName.size() != 0) {
        optionSeperator();

        optionDash(false);
        optionName(opt.shortName);   
    }
}

std::size_t 
HelpFormatter::optionHelpNameLength(Option const& opt)
{
    size_t length = 2 + opt.longName.size();
    if(opt.shortName.size() != 0) {
        length += 3 + opt.shortName.size();
    }
    return length;
}

void
HelpFormatter::appendText(
        std::string value,
        bool handleFormatting
    )
{
    if(handleFormatting) {
        mCurrLineLength = formatAndAppendText(
                mHelpString, 
                mCurrLineLength,
                mCurrIndentAmount,
                mMaxLineWidth,
                value
            );
    } 
    else {
        // For unformatted text, simply append and update the current line length.
        mHelpString += value;
        mCurrLineLength += value.size();
    }
}

void
HelpFormatter::newLine() 
{
    mHelpString += '\n';
    mCurrLineLength = 0;
}

void 
HelpFormatter::indent(std::size_t amount)
{
    appendText(std::string(amount, ' '));
}

void 
DefaultHelpFormatter::resetColor()
{
    if(mIsTTY) {
        mHelpString += color::ResetColor;
    }
}

void
DefaultHelpFormatter::generateCommandHelp(
        CommandPtr com, 
        int maxOptComLength
    )
{
    commandName(com->name);
            
    if(com->description != "") {
        // Justify the description.
        if(com->name.size() < maxOptComLength) {
            indent(maxOptComLength - com->name.size());
        }

        mCurrIndentAmount = maxOptComLength + mStyle.initialIndentLevel + mStyle.separator.size();
        seperator();
        commandDescription(com->description);
    }

    newLine();

    // Print any command specific options with an extra level of indentation
    for(auto opt : com->options.values()) 
    {
        indent(mStyle.initialIndentLevel + mStyle.spacesPerIndentLevel);
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        if(opt.description != "") {

            // Justify the description.
            if((optLen+mStyle.spacesPerIndentLevel) < maxOptComLength) {
                indent(maxOptComLength - optLen - mStyle.spacesPerIndentLevel);
            }

            mCurrIndentAmount = maxOptComLength + mStyle.initialIndentLevel + mStyle.separator.size();
            seperator();
            optionDescription(opt.description);
        }
        newLine();
    }
}

void
DefaultHelpFormatter::generateSubParserHelp(
        SubParserPtr com, 
        int maxOptComLength
    )
{
    commandName(com->name);
            
    // Print the sub parser description if it exists
    if(com->description != "") {

        // Justify the description.
        if(com->name.size() < maxOptComLength) {
            indent(maxOptComLength - com->name.size());
        }

        mCurrIndentAmount = maxOptComLength + mStyle.initialIndentLevel + mStyle.separator.size();
        seperator();
        commandDescription(com->description);
    }

    newLine();

    // Print any command specific options with an extra level of indentation
    for(auto opt : com->options.values()) 
    {
        indent(mStyle.initialIndentLevel + mStyle.spacesPerIndentLevel);
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Print the option description
        if(opt.description != "") {

            // Justify the description.
            if((optLen+mStyle.spacesPerIndentLevel) < maxOptComLength) {
                indent(maxOptComLength - optLen - mStyle.spacesPerIndentLevel);
            }

            mCurrIndentAmount = maxOptComLength + mStyle.initialIndentLevel + mStyle.separator.size();
            seperator();
            optionDescription(opt.description);
        }
        newLine();
    }
}

DefaultHelpFormatter::DefaultHelpFormatter(
        Parser& parser,
        DefaultFormatStyle style,
        bool forceNoColor
    )
    : mParser(parser),
      mStyle(style)
{
    mMaxOptComLength = std::max(findMaxOptComLength(parser, style.spacesPerIndentLevel), style.maxJustified);
    if(forceNoColor) {
        mIsTTY = false;
    }
    else {
        mIsTTY = isatty(fileno(stdout)) != 0;
    }
    mMaxLineWidth = std::min(style.maxLineLength, displayWidth()-1);
}

void 
DefaultHelpFormatter::programName(std::string name)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.programNameColor;
    }
    
    appendText(name);
    resetColor();
    newLine();
}

void 
DefaultHelpFormatter::beginGroup(std::string value)
{
    newLine();

    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.groupNameColor;
    }
    
    appendText(value + mStyle.groupNameSuffix);
    resetColor();
    newLine();
}

void 
DefaultHelpFormatter::endGroup()
{
    mHelpString += "\n";
}

void 
DefaultHelpFormatter::commandName(std::string key)
{
    indent(mStyle.initialIndentLevel);
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.commandNameColor;
    }
    
    appendText(key);
    resetColor();
}

void 
DefaultHelpFormatter::optionName(std::string key)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.optionNameColor;
    }
    
    appendText(key);
    resetColor();
}

void 
DefaultHelpFormatter::optionDash(bool longDash)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.optionDashColor;
    }
    
    if(longDash) {
        appendText("--");
    }
    else {
        appendText("-");
    }

    resetColor();
}

void 
DefaultHelpFormatter::optionSeperator()
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.optionSeparatorColor;
    }
    
    appendText(mStyle.optionSeparator);
    resetColor();
}

void 
DefaultHelpFormatter::seperator()
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.separatorColor;
    }
    
    appendText(mStyle.separator);
    resetColor();
}

void 
DefaultHelpFormatter::programDescription(std::string value)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.programDescriptionColor;
    }
    
    appendText(value, true);
    newLine();
    newLine();
    resetColor();
}

void 
DefaultHelpFormatter::programUsage(std::string value)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.programUsageColor;
    }
    
    appendText(value, true);
    newLine();
    resetColor();
}

void 
DefaultHelpFormatter::groupDescription(std::string value)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.groupDescriptionColor;
    }
    
    appendText(value, true);
    resetColor();
}

void 
DefaultHelpFormatter::commandDescription(std::string value)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.commandDescriptionColor;
    }
    
    appendText(value, true);
    resetColor();
}

void 
DefaultHelpFormatter::optionDescription(std::string value)
{
    if(mIsTTY) {
        resetColor();
        mHelpString += mStyle.optionDescriptionColor;
    }
    
    appendText(value, true);
    resetColor();
}

std::size_t 
HelpFormatter::findMaxOptComLength(Parser& parser, std::size_t indentPerLevel)
{
    // First find the max length of option/command pieces
    std::size_t maxOptComLength = 0;
    for(auto opt : parser.mOptions.values()) {
        maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt));
    }

    for(auto com : parser.mCommands) {
        maxOptComLength = std::max(maxOptComLength, com->name.size());
        for(auto opt : com->options.values()) {
            maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt) + indentPerLevel);
        }
    }

    // Check across grouped commands as well.
    for(const auto& group : parser.mGroups) {
        for(auto com : group.commands) {
            maxOptComLength = std::max(maxOptComLength, com->name.size());
            for(auto opt : com->options.values()) {
                maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt) + indentPerLevel);
            }
        }
    }

    return maxOptComLength;
}

std::string 
DefaultHelpFormatter::helpString()
{
    mHelpString = "";
    mCurrLineLength = 0;

    if(mParser.mBanner != "") {
        programName(mParser.mBanner);
    }
    else {
        programName(mParser.mName);
    }

    if(mParser.mDescription != "") {
        programDescription(mParser.mDescription);
    }

    if(mParser.mUsage != "") {
        programUsage(mParser.mUsage);
    }

    // Now build up the help string.
    beginGroup("Global Options");
    for(auto opt : mParser.mOptions.values()) {
        indent(mStyle.initialIndentLevel);
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if(optLen < mMaxOptComLength) {
            indent(mMaxOptComLength - optLen);
        }

        if(opt.description != "") {
            mCurrIndentAmount = mMaxOptComLength + mStyle.initialIndentLevel + mStyle.separator.size();
            seperator();
            optionDescription(opt.description);
        }

        newLine();
    }

    if( mParser.mCommands.size() > 0 )
    {
        beginGroup("Commands");

        for(auto com : mParser.mCommands) {
            generateCommandHelp(com, mMaxOptComLength);
        }
        
        for(auto sub : mParser.mSubParsers) {
            generateSubParserHelp(sub, mMaxOptComLength);
        }
    }

    if(mParser.mGroups.size() > 0) {
        for(const auto& group : mParser.mGroups) 
        {
            beginGroup(group.name);
            if(group.description != "") {

                indent(mStyle.initialIndentLevel);
                mCurrIndentAmount = mStyle.initialIndentLevel;

                groupDescription(group.description);

                newLine();
                newLine();
            }

            for(auto com : group.commands) {
                generateCommandHelp(com, mMaxOptComLength);
            }

            for(auto sub : group.subParsers) {
                generateSubParserHelp(sub, mMaxOptComLength);
            }
        }
    }

    newLine();
    return mHelpString;
}

}
