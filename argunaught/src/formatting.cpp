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

size_t 
HelpFormatter::optionHelpNameLength(Option const& opt)
{
    size_t length = 2 + opt.longName.size();
    if(opt.shortName.size() != 0) {
        length += 3 + opt.shortName.size();
    }
    return length;
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
        mHelpString += std::string(maxOptComLength - com->name.size(), ' '); 
    }
    if(com->help != "") {
        // Replace new lines in help with justified new lines
        auto indentLength = maxOptComLength + 4 + 3;
        auto indent = "\n" + std::string(indentLength, ' ');
        auto comHelp = replaceAll(com->help, '\n', indent);
        //help += " - " + comHelp;
        seperator();
        commandDescription(comHelp);
    }

    mHelpString += "\n";

    for(auto opt : com->options.values()) {
        mHelpString += std::string(initialIndentLevel + spacesPerIndentLevel, ' ');
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if((optLen+spacesPerIndentLevel) < maxOptComLength) {
            mHelpString += std::string(maxOptComLength - optLen - spacesPerIndentLevel, ' '); 
        }

        if(opt.description != "") {
            auto indentLength = maxOptComLength + 6 + 3;
            auto indent = "\n" + std::string(indentLength, ' ');
            auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += " - " + optDesc;
            seperator();
            optionDescription(optDesc);
        }
        mHelpString += "\n";
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
        mHelpString += std::string(maxOptComLength - com->name.size(), ' '); 
    }
    if(com->help != "") {
        // Replace new lines in help with justified new lines
        auto indentLength = maxOptComLength + 4 + 3;
        auto indent = "\n" + std::string(indentLength, ' ');
        auto comHelp = replaceAll(com->help, '\n', indent);
        //help += " - " + comHelp;
        seperator();
        commandDescription(comHelp);
    }

    mHelpString += "\n";

    for(auto opt : com->options.values()) {
        mHelpString += std::string(initialIndentLevel + spacesPerIndentLevel, ' ');
        optionHelpName(opt);
        auto optLen = optionHelpNameLength(opt);

        // Justify the description.
        if((optLen+spacesPerIndentLevel) < maxOptComLength) {
            mHelpString += std::string(maxOptComLength - optLen - spacesPerIndentLevel, ' '); 
        }

        if(opt.description != "") {
            auto indentLength = maxOptComLength + 6 + 3;
            auto indent = "\n" + std::string(indentLength, ' ');
            auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += " - " + optDesc;
            seperator();
            optionDescription(optDesc);
        }
        mHelpString += "\n";
    }
}

DefaultHelpFormatter::DefaultHelpFormatter(Parser& parser)
    : mParser(parser)
{
    mMaxOptComLength = std::max(findMaxOptComLength(parser), (size_t)maxJustified);
    mIsTTY = true;//isatty(fileno(stdout)) == 0;
}

void 
DefaultHelpFormatter::programName(std::string name)
{
    if(mIsTTY) {
        mHelpString += color::foreground::BoldGreenColor;
    }
    mHelpString += name + "\n";
}

void 
DefaultHelpFormatter::beginGroup(std::string value)
{

    if(mIsTTY) {
        mHelpString += color::foreground::BoldBlueColor;
    }
    
    mHelpString += "\n" + value + ":\n";
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
    
    mHelpString += key;
}

void 
DefaultHelpFormatter::optionName(std::string key)
{
    if(mIsTTY) {
        mHelpString += color::foreground::BoldCyanColor;
    }
    
    mHelpString += key;
}

void 
DefaultHelpFormatter::optionDash(bool longDash)
{
    if(mIsTTY) {
        mHelpString += color::foreground::CyanColor;
    }
    
    if(longDash) {
        mHelpString += "--";
    }
    else {
        mHelpString += "-";
    }
}

void 
DefaultHelpFormatter::optionSeperator()
{
    if(mIsTTY) {
        mHelpString += color::foreground::CyanColor;
    }
    
    mHelpString += ", ";
}

void 
DefaultHelpFormatter::seperator()
{
    if(mIsTTY) {
        mHelpString += color::foreground::GrayColor;
    }
    
    mHelpString += keyValSep;
}

void 
DefaultHelpFormatter::commandDescription(std::string value)
{
    if(mIsTTY) {
        mHelpString += color::foreground::WhiteColor;
    }
    
    mHelpString += value;

}

void 
DefaultHelpFormatter::optionDescription(std::string value)
{
    if(mIsTTY) {
        mHelpString += color::foreground::WhiteColor;
    }
    
    mHelpString += value;

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
            auto indentLength = mMaxOptComLength + initialIndentLevel + keValSepLength;
            auto indent = "\n" + std::string(indentLength, ' ');
            auto optDesc = replaceAll(opt.description, '\n', indent);
            //mHelpString += keyValSep + optDesc;
            seperator();
            optionDescription(optDesc);
        }
        mHelpString += "\n";
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

    mHelpString += "\n";
    return mHelpString;
}

}
