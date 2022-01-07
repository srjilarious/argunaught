#include <argunaught/argunaught.hpp>

#include <unistd.h>

namespace argunaught
{

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

Command::Command(
        std::string n, 
        std::string h, 
        std::vector<Option> opt, 
        CommandHandler f//, 
        // bool _handlesSubParsers
    )
    : name(n), help(h), options(opt), handler(f)//, handlesSubParsers(_handlesSubParsers)
{
}

SubParser::SubParser(
        std::string n, 
        std::string h, 
        std::vector<Option> opt, 
        SubParserHandler f
    )
    : name(n), help(h), options(opt), handler(f)
{
}

Parser::Parser(std::string name, std::string banner)
    : mName(name), mBanner(banner)
{
}

Parser& 
Parser::command(
        std::string name, 
        std::string help, 
        CommandHandler func)//,
        // bool handlesSubParsers)
{
    mCommands.push_back(std::shared_ptr<Command>(new Command(name, help, {}, func)));
    return *this;    
}

Parser& 
Parser::command(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        CommandHandler func
    )
{
    mCommands.push_back(std::make_shared<Command>(name, help, options, func));
    return *this;
}

Parser& 
Parser::options(
        std::vector<Option> options
    )
{
    for(auto opt : options) {
        mOptions.addOption(opt);
    }
    return *this;
}

Parser& 
Parser::options(
        const OptionList& opts
    )
{
    return options(opts.values());
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
        }
    }

    mHelpString += "\n";
    return mHelpString;
}


OptionList::OptionList(std::vector<Option> opts)
{
    for(auto& opt : opts) {
        ARGUNAUGHT_TRACE("Adding option -%s, --%s\n", opt.shortName.c_str(), opt.longName.c_str());
        addOption(opt);
    }
}

OptionList::OptionList(const OptionList& opts)
    : OptionList(opts.mOptions)
{
}

OptionError 
OptionList::addOption(Option opt)
{
    // FIX: Currently no duplicate checks.
    mOptions.push_back(opt);
    return OptionError::None;
}

OptionError 
OptionList::addOptions(const OptionList& opts)
{
    // FIX: Currently no duplicate checks.
    mOptions.insert(mOptions.end(), opts.mOptions.begin(), opts.mOptions.end());
    return OptionError::None;
}

std::optional<Option> 
OptionList::findShortOption(std::string optionName) const
{
    ARGUNAUGHT_TRACE("Looking for short option, have %d options\n", mOptions.size());
    auto it = std::find_if(std::begin(mOptions), std::end(mOptions), [optionName] (const Option& opt) {
        ARGUNAUGHT_TRACE("Checking '%s' against '%s'\n", optionName.c_str(), opt.shortName.c_str());
        return opt.shortName == optionName;
    });

    if(it != mOptions.end()) {
        return std::optional<Option>(*it);
    }

    return std::nullopt;
}

std::optional<Option> 
OptionList::findLongOption(std::string optionName) const
{
    auto it = std::find_if(std::begin(mOptions), std::end(mOptions), [optionName] (const Option& opt) {
        return opt.longName == optionName;
    });

    if(it != mOptions.end()) {
        ARGUNAUGHT_TRACE("Found long option in parser.");
        return std::optional<Option>(*it);
    }

    return std::nullopt;
}

int 
ParseResult::runCommand() const
{
    if(command) {
        return command->handler(*this);
    }

    return -1;
}

std::optional<OptionResult> 
ParseResult::getOption(std::string optionLongName) const
{
    std::optional<OptionResult> result = std::nullopt;
    auto found = std::find_if(options.begin(), options.end(), [&](auto val) {
        return val.optionName == optionLongName;
    });
    if(found != options.end()) {
        result = *found;
    }
    return result;
}

bool 
ParseResult::hasOption(std::string optionLongName) const
{
    return getOption(optionLongName) != std::nullopt;
}

std::optional<OptionResult>
Parser::parseOption(std::shared_ptr<Command> command, 
                    std::deque<std::string>& parseText,
                    ParseResult& parseResult) const
{
    OptionResult optResult;

    // Expect atleast one value in parseText
    if(parseText.size() == 0) return std::nullopt;

    // Get the option name out.
    std::string optionFullName = parseText[0];

    // Make sure the option is not just a - or --
    if(optionFullName == "-" || 
       optionFullName == "--") 
    {
           parseText.pop_front();
           return std::nullopt;
    }

    std::string optionName;
    std::optional<Option> opt;

    // Check for a long name
    if(optionFullName[0] == '-' && optionFullName[1] == '-') {
        // Skip over '--'
        optionName = optionFullName.substr(2);
        ARGUNAUGHT_TRACE("Got long option name: '%s'\n", optionName.c_str());
        if(command != nullptr) {
            opt = command->options.findLongOption(optionName);
        }

        if(!opt.has_value()) {
            ARGUNAUGHT_TRACE("No command option, checking for global option.\n");
            opt = mOptions.findLongOption(optionName);
        }
    }
    else if(optionFullName[0] == '-') {
        // Skip over '-'
        optionName = optionFullName.substr(1);
        ARGUNAUGHT_TRACE("Got short option name: '%s'\n", optionName.c_str());

        if(command != nullptr) {
            opt = command->options.findShortOption(optionName);
        }

        if(!opt.has_value()) {
            ARGUNAUGHT_TRACE("No command option, checking for global option.\n");
            opt = mOptions.findShortOption(optionName);
        }
    }
    else {
        return std::nullopt;
    }

    parseText.pop_front();
    ARGUNAUGHT_TRACE("Option found: %s\n", opt.has_value() ? "True" : "False");
    if(opt.has_value()) {
        Option& foundOption = opt.value();
        optResult.optionName = foundOption.longName;

        int paramCounter = 0;

        ARGUNAUGHT_TRACE("Checking for option values.\n");

        // Parse any values until the next option.
        while(!parseText.empty() && 
              (foundOption.maxNumParams == -1 || 
               paramCounter < foundOption.maxNumParams) &&
              parseText.front()[0] != '-')
        {
            ARGUNAUGHT_TRACE("Got option value: '%s'\n", parseText.front().c_str());
            optResult.values.push_back(parseText.front());
            parseText.pop_front();
            paramCounter++; 
        }
    
        ARGUNAUGHT_TRACE("Done checking for option values. %d found\n", paramCounter);
        return std::optional<OptionResult>(optResult);
    } else {
        parseResult.errors.push_back({ParseErrorType::UnknownOption, -1, optionName});
    }

    return std::nullopt;
}

CommandGroup::CommandGroup(
        Parser* parent, 
        std::string _name, 
        std::string _desc
    ) : mParent(parent),
        name(_name),
        description(_desc)
{
}

CommandGroup& 
Parser::group(std::string name)
{
    mGroups.push_back(CommandGroup(this, name));
    return *(mGroups.end()-1);
}

CommandGroup& 
CommandGroup::command(
        std::string name, 
        std::string help, 
        CommandHandler func)
{
    commands.push_back(std::shared_ptr<Command>(new Command(name, help, {}, func)));
    return *this;    
}

CommandGroup& 
CommandGroup::command(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        CommandHandler func
    )
{
    commands.push_back(std::make_shared<Command>(name, help, options, func));
    return *this;
}

Parser& 
Parser::subParser(
        std::string name, 
        std::string help, 
        SubParserHandler func)
{
    mSubParsers.push_back(std::shared_ptr<SubParser>(new SubParser(name, help, {}, func)));
    return *this; 
}

Parser& 
Parser::subParser(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        SubParserHandler func
    )
{
    mSubParsers.push_back(std::shared_ptr<SubParser>(new SubParser(name, help, options, func)));
    return *this; 
}


ParseResult
Parser::parse(int argc, const char* argv[]) const
{
    // Skip the executable name
    argv = &argv[1];
    argc--;

    // Create a deque of strings
    std::deque<std::string> args;
    for(size_t ii = 0; ii < argc; ++ii)
    {
        args.emplace_back(argv[ii]);
    }

    return parse(args);
}

ParseResult
Parser::parse(ParseResult const& prevParseResult)
{
    // Create a deque of strings
    std::deque<std::string> args;
    for(auto& arg : prevParseResult.positionalArgs)
    {
        args.emplace_back(arg);
    }

    mOptions.addOptions(prevParseResult.optionsList.values());
    return parse(args);
}

ParseResult
Parser::parse(std::deque<std::string> args, OptionResultList existingOptions) const
{
    ParseResult result;

    for(const auto& opt : existingOptions) {
        result.options.push_back(opt);
    }

    if(args.size() == 0) return result;

    result.optionsList.addOptions(mOptions);

    // parse any options before the command as global options
    while(!args.empty() && args.front()[0] == '-') {
        auto optResult = parseOption(nullptr, args, result);
        if(optResult.has_value()) {
            result.options.push_back(optResult.value());
        }
        else {
            break;
        }
    }

    // Check for just options, no command.
    if(args.size() == 0) return result;

    // Create a combined list of un-grouped commands and grouped commands
    CommandList allCommands;
    std::copy(mCommands.begin(), mCommands.end(), std::back_inserter(allCommands));
    for(const auto& group : mGroups) {
        std::copy(
            group.commands.begin(), 
            group.commands.end(), 
            std::back_inserter(allCommands)
        );
    }

    for(const auto& com : allCommands) {
        if(com->name == args[0]) {
            ARGUNAUGHT_TRACE("Found command '%s'\n", com->name.c_str());
            result.command = com;

            args.pop_front();

            result.optionsList.addOptions(com->options);

            while(!args.empty() && args.front()[0] == '-') {
                auto optResult = parseOption(com, args, result);
                if(optResult.has_value()) {
                    result.options.push_back(optResult.value());
                }
                else {
                    break;
                }

                ARGUNAUGHT_TRACE("Done parsing option '%s', ended with %d params\n",
                optResult.value().optionName.c_str(), optResult.value().values.size());
            }

            break;
        }
    }
    
    for(const auto& subCom : mSubParsers) {
        if(subCom->name == args[0]) {
            ARGUNAUGHT_TRACE("Found sub command '%s'\n", com->name.c_str());
            
            args.pop_front();
            result = subCom->handler(*this, result.options, args);
            
            return result;
        }
    }

    // Fall through in case of no command, sub command, or remaining args for command.
    ARGUNAUGHT_TRACE("Checking positional args, %d left", args.size());

    // Anything left over is a positional argument.
    while(!args.empty()) {
        ARGUNAUGHT_TRACE("Got positional arg: '%s'\n", args.front().c_str());
        result.positionalArgs.push_back(args.front());
        args.pop_front();
    }
    return result;
}

}
