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
        // TODO: Handle max allowed line < curr indent amount
        // TODO: Handle hyphenating
        // TODO: Handle new lines again.

        std::size_t start = 0;
        std::size_t prevWordLoc = 0;
        std::size_t nextWordLoc = 0;
        std::size_t currWriteLen = 0;

        while(true) {
            prevWordLoc = nextWordLoc;
            nextWordLoc = value.find(' ', prevWordLoc);
            if(nextWordLoc == std::string::npos) {
                currWriteLen = value.size() - start;
                nextWordLoc = value.size();
            }
            else {
                currWriteLen = nextWordLoc - start;
                nextWordLoc++;
            }

            if((mCurrLineLength + currWriteLen) >= mMaxLineWidth) {
                // if(start == prevWordLoc) {
                //     // hyphenate here.
                // }

                // word wrap break
                mHelpString += value.substr(start, prevWordLoc - start) + "\n";
                mHelpString += std::string(mCurrIndentAmount, ' ');
                mCurrLineLength = mCurrIndentAmount;
                currWriteLen = 0;
                start = prevWordLoc;
            }
            
            if(nextWordLoc >= value.size()) {
                mHelpString += value.substr(start, currWriteLen);
                mCurrLineLength += currWriteLen;
                break;
            }
        }

        //auto splitLines = split(value, '\n');

        // TODO: Implement line wrapping on word boundaries.
        // for(auto& s : splitLines) {
        //     if(mCurrLineLength + s.size() > mMaxLineWidth) {
        //         mHelpString += "\n";
        //         mHelpString += std::string(mCurrIndentAmount, ' ');
        //         mHelpString += 
        //     }
        // }
    } else {
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
DefaultHelpFormatter::generateCommandHelp(
        CommandPtr com, 
        int maxOptComLength
    )
{
    commandName(com->name);
            
    // Justify the description.
    if(com->name.size() < maxOptComLength) {
        appendText(std::string(maxOptComLength - com->name.size(), ' '));
    }
    if(com->help != "") {
        mCurrIndentAmount = maxOptComLength + 4 + keyValSep.size();
        // Replace new lines in help with justified new lines
        //auto indentLength = maxOptComLength + 4 + 3;
        //auto indent = "\n" + std::string(indentLength, ' ');
        //auto comHelp = replaceAll(com->help, '\n', indent);
        seperator();
        commandDescription(com->help);
    }

    newLine();

    for(auto opt : com->options.values()) {
        appendText(std::string(initialIndentLevel + spacesPerIndentLevel, ' '));
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if((optLen+spacesPerIndentLevel) < maxOptComLength) {
            appendText(std::string(maxOptComLength - optLen - spacesPerIndentLevel, ' '));
        }

        if(opt.description != "") {
            mCurrIndentAmount = maxOptComLength + 4 + keyValSep.size();
            //auto indentLength = maxOptComLength + 6 + keyValSep.size();
            //auto indent = "\n" + std::string(indentLength, ' ');
            //auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += " - " + optDesc;
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
            
    // Justify the description.
    if(com->name.size() < maxOptComLength) {
        appendText(std::string(maxOptComLength - com->name.size(), ' '));
    }
    if(com->help != "") {
        mCurrIndentAmount = maxOptComLength + 4 + keyValSep.size();
        // auto indentLength = maxOptComLength + 4 + keyValSep.size();
        // auto indent = "\n" + std::string(indentLength, ' ');
        // auto comHelp = replaceAll(com->help, '\n', indent);
        //help += " - " + comHelp;
        seperator();
        commandDescription(com->help);
    }

    newLine();

    for(auto opt : com->options.values()) {
        appendText(std::string(initialIndentLevel + spacesPerIndentLevel, ' '));
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if((optLen+spacesPerIndentLevel) < maxOptComLength) {
            appendText(std::string(maxOptComLength - optLen - spacesPerIndentLevel, ' '));
        }

        if(opt.description != "") {
            mCurrIndentAmount = maxOptComLength + 4 + keyValSep.size();
            // auto indentLength = maxOptComLength + 6 + keyValSep.size();
            // auto indent = "\n" + std::string(indentLength, ' ');
            // auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += " - " + optDesc;
            seperator();
            optionDescription(opt.description);
        }
        newLine();
    }
}

DefaultHelpFormatter::DefaultHelpFormatter(Parser& parser)
    : mParser(parser)
{
    mMaxOptComLength = std::max(findMaxOptComLength(parser), (size_t)maxJustified);
    mIsTTY = isatty(fileno(stdout)) != 0;
    mMaxLineWidth = displayWidth()-1;
}

void 
DefaultHelpFormatter::programName(std::string name)
{
    if(mIsTTY) {
        mHelpString += color::foreground::BoldGreenColor;
    }
    
    appendText(name);
    newLine();
}

void 
DefaultHelpFormatter::beginGroup(std::string value)
{

    if(mIsTTY) {
        mHelpString += color::foreground::BoldBlueColor;
    }
    
    
    newLine();
    appendText(value + ":");
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
    mHelpString += std::string(initialIndentLevel, ' ');
    if(mIsTTY) {
        mHelpString += color::foreground::BoldYellowColor;
    }
    
    appendText(key);
}

void 
DefaultHelpFormatter::optionName(std::string key)
{
    if(mIsTTY) {
        mHelpString += color::foreground::BoldCyanColor;
    }
    
    appendText(key);
}

void 
DefaultHelpFormatter::optionDash(bool longDash)
{
    if(mIsTTY) {
        mHelpString += color::foreground::CyanColor;
    }
    
    if(longDash) {
        appendText("--");
        
    }
    else {
        appendText("-");
    }
}

void 
DefaultHelpFormatter::optionSeperator()
{
    if(mIsTTY) {
        mHelpString += color::foreground::CyanColor;
    }
    
    appendText(", ");
}

void 
DefaultHelpFormatter::seperator()
{
    if(mIsTTY) {
        mHelpString += color::foreground::GrayColor;
    }
    
    appendText(keyValSep);
}

void 
DefaultHelpFormatter::commandDescription(std::string value)
{
    if(mIsTTY) {
        mHelpString += color::foreground::WhiteColor;
    }
    
    appendText(value, true);
}

void 
DefaultHelpFormatter::optionDescription(std::string value)
{
    if(mIsTTY) {
        mHelpString += color::foreground::WhiteColor;
    }
    
    appendText(value, true);
}

std::size_t 
HelpFormatter::findMaxOptComLength(Parser& parser)
{
    // First find the max length of option/command pieces
    std::size_t maxOptComLength = 0;
    for(auto opt : parser.mOptions.values()) {
        maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt));
    }

    for(auto com : parser.mCommands) {
        maxOptComLength = std::max(maxOptComLength, com->name.size());
        for(auto opt : com->options.values()) {
            // + 2 for the indent on top of normal indentation.
            maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt) + 2);
        }
    }

    // Check across grouped commands as well.
    for(const auto& group : parser.mGroups) {
        for(auto com : group.commands) {
            maxOptComLength = std::max(maxOptComLength, com->name.size());
            for(auto opt : com->options.values()) {
                // + 2 for the indent on top of normal indentation.
                maxOptComLength = std::max(maxOptComLength, optionHelpNameLength(opt)+ 2);
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

    // Now build up the help string.
    beginGroup("Global Options");
    for(auto opt : mParser.mOptions.values()) {
        mHelpString += initialIndent;
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if(optLen < mMaxOptComLength) {
            mHelpString += std::string(mMaxOptComLength - optLen, ' '); 
        }

        if(opt.description != "") {
            mCurrIndentAmount = mMaxOptComLength + initialIndentLevel + keyValSep.size();
            //auto indent = "\n" + std::string(indentLength, ' ');
            //auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += keyValSep + optDesc;
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
                mHelpString += std::string(spacesPerIndentLevel, ' ') + group.description + "\n\n";
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
