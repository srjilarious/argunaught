#include <argunaught/argunaught.hpp>

#include <unistd.h>

namespace argunaught
{

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


CommandGroup& 
Parser::group(std::string name)
{
    mGroups.push_back(CommandGroup(this, name));
    return *(mGroups.end()-1);
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

CommandGroup& 
CommandGroup::subParser(
        std::string name, 
        std::string help, 
        SubParserHandler func)
{
    subParsers.push_back(std::shared_ptr<SubParser>(new SubParser(name, help, {}, func)));
    return *this; 
}

CommandGroup& 
CommandGroup::subParser(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        SubParserHandler func
    )
{
    subParsers.push_back(std::shared_ptr<SubParser>(new SubParser(name, help, options, func)));
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
    
    // Create a combined list of un-grouped commands and grouped commands
    SubParserList allSubParsers;
    std::copy(mSubParsers.begin(), mSubParsers.end(), std::back_inserter(allSubParsers));
    for(const auto& group : mGroups) {
        std::copy(
            group.subParsers.begin(), 
            group.subParsers.end(), 
            std::back_inserter(allSubParsers)
        );
    }

    for(const auto& subCom : allSubParsers) {
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
