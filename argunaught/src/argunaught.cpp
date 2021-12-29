#include <argunaught/argunaught.hpp>

namespace argunaught
{

Command::Command(
        std::string n, 
        std::string h, 
        std::vector<Option> opt, 
        CommandHandler f, 
        bool _handlesSubCommands
    )
    : name(n), help(h), options(opt), handler(f), handlesSubCommands(_handlesSubCommands)
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
        CommandHandler func,
        bool handlesSubCommands)
{
    mCommands.push_back(std::shared_ptr<Command>(new Command(name, help, {}, func, handlesSubCommands)));
    return *this;    
}

Parser& 
Parser::command(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        CommandHandler func,
        bool handlesSubCommands
    )
{
    mCommands.push_back(std::make_shared<Command>(name, help, options, func, handlesSubCommands));
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
        OptionList opts
    )
{
    return options(opts.values());
}

std::string 
Parser::optionHelpName(Option const& opt) const
{
    if(opt.shortName.size() != 0) {
        return "--" + opt.longName + ", -" + opt.shortName;
    }
    else {
        return "--" + opt.longName;
    }
}

std::string 
Parser::replaceAll(std::string const& orig, char c, std::string const& replace) const
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

std::string 
Parser::help(std::size_t minJustified, std::size_t maxJustified) const
{
    std::string help;
    if(mBanner != "") {
        help = mBanner + "\n";
    }
    else {
        help = mName + "\n";
    }
    
    // First find the max length of option/command pieces
    std::size_t maxOptComLength = 0;
    for(auto opt : mOptions.values()) {
        maxOptComLength = std::max(maxOptComLength, optionHelpName(opt).size());
    }

    for(auto com : mCommands) {
        maxOptComLength = std::max(maxOptComLength, com->name.size());
        for(auto opt : com->options.values()) {
            // + 2 for the indent on top of normal indentation.
            maxOptComLength = std::max(maxOptComLength, optionHelpName(opt).size() + 2);
        }
    }

    maxOptComLength = std::max(maxOptComLength, maxJustified);

    // Now build up the help string.
    help += "\nGlobal Options:\n";
    for(auto opt : mOptions.values()) {
        auto optName = optionHelpName(opt);

        help += "    " + optName;

        // Justify the description.
        if(optName.size() < maxOptComLength) {
            help += std::string(maxOptComLength - optName.size(), ' '); 
        }

        if(opt.description != "") {
            auto indentLength = maxOptComLength + 4 + 3;
            auto indent = "\n" + std::string(indentLength, ' ');
            auto optDesc = replaceAll(opt.description, '\n', indent);
            help += " - " + optDesc;
        }
        help += "\n";
    }

    if( mCommands.size() > 0 )
    {
        help += "\nCommands:\n";

        for(auto com : mCommands) {
            help += "    " + com->name;
            
            // Justify the description.
            if(com->name.size() < maxOptComLength) {
                help += std::string(maxOptComLength - com->name.size(), ' '); 
            }
            if(com->help != "") {
                // Replace new lines in help with justified new lines
                auto indentLength = maxOptComLength + 4 + 3;
                auto indent = "\n" + std::string(indentLength, ' ');
                auto comHelp = replaceAll(com->help, '\n', indent);
                help += " - " + comHelp;
            }

            help += "\n";

            for(auto opt : com->options.values()) {
                auto optName = optionHelpName(opt);

                help += "      " + optName;

                // Justify the description.
                if((optName.size()+2) < maxOptComLength) {
                    help += std::string(maxOptComLength - optName.size() - 2, ' '); 
                }

                if(opt.description != "") {
                    auto indentLength = maxOptComLength + 6 + 3;
                    auto indent = "\n" + std::string(indentLength, ' ');
                    auto optDesc = replaceAll(opt.description, '\n', indent);
                    help += " - " + optDesc;
                }
                help += "\n";
            }
        }
    }

    help += "\n";
    return help;
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

ParseResult
Parser::parse(int argc, const char* argv[], OptionResultList existingOptions) const
{
    ParseResult result;

    // Skip the executable name
    argv = &argv[1];
    argc--;

    // Create a vector of strings
    std::deque<std::string> args;
    for(size_t ii = 0; ii < argc; ++ii)
    {
        args.emplace_back(argv[ii]);
    }

    if(args.size() == 0) return result;

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

    for(const auto& com : mCommands) {
        if(com->name == args[0]) {
            ARGUNAUGHT_TRACE("Found command '%s'\n", com->name.c_str());
            result.command = com;

            args.pop_front();

            if(com->handlesSubCommands) {
                result.subCommandInfo.remainingArgs = args;
                result.subCommandInfo.prevOptions.addOptions(mOptions);
                result.subCommandInfo.prevOptions.addOptions(com->options);
                return result;
            }

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
