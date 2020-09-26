#include "argunaught.hpp"

namespace argunaught
{


Command::Command(
        std::string n, 
        std::vector<Option> opt, 
        CommandHandler f
    )
    : name(n), options(opt), handler(f)
{
}

Parser::Parser()
{

}

Parser& 
Parser::command(
        std::string name, 
        CommandHandler func)
{
    return *this;    
}

Parser& 
Parser::command(
        std::string name, 
        std::vector<Option> options, 
        CommandHandler func
    )
{
    mCommands.push_back(std::make_shared<Command>(name, options, func));
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

OptionList::OptionList(std::vector<Option> opts)
{
    for(auto& opt : opts) {
        printf("Adding option -%s, --%s\n", opt.shortName.c_str(), opt.longName.c_str());
        addOption(opt);
    }
}

OptionError 
OptionList::addOption(Option opt)
{
    mOptions.push_back(opt);
}

std::optional<Option> 
OptionList::findShortOption(std::string optionName) const
{
    printf("Looking for short option, have %d options\n", mOptions.size());
    auto it = std::find_if(std::begin(mOptions), std::end(mOptions), [optionName] (const Option& opt) {
        printf("Checking '%s' against '%s'\n", optionName.c_str(), opt.shortName.c_str());
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
        printf("Found short option in parser.");
        return std::optional<Option>(*it);
    }

    return std::nullopt;
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
           return std::nullopt;
    }

    std::string optionName;
    std::optional<Option> opt;

    // Check for a long name
    if(optionFullName[1] == '-') {
        // Skip over '--'
        optionName = optionFullName.substr(2);
        printf("Got long option name: '%s'\n", optionName.c_str());
        if(command != nullptr) {
            opt = command->options.findLongOption(optionName);
        }

        if(!opt.has_value()) {
            printf("No command option, checking for global option.\n");
            opt = mOptions.findLongOption(optionName);
        }
    }
    else {
        // Skip over '-'
        optionName = optionFullName.substr(1);
        printf("Got short option name: '%s'\n", optionName.c_str());

        if(command != nullptr) {
            opt = command->options.findShortOption(optionName);
        }

        if(!opt.has_value()) {
            printf("No command option, checking for global option.\n");
            opt = mOptions.findShortOption(optionName);
        }
    }

    parseText.pop_front();
    printf("Option found: %s\n", opt.has_value() ? "True" : "False");
    if(opt.has_value()) {
        Option& foundOption = opt.value();
        optResult.optionName = foundOption.longName;

        int paramCounter = 0;

        // Parse any values until the next option.
        while(!parseText.empty() && 
              (foundOption.maxNumParams == -1 || 
               paramCounter < foundOption.maxNumParams) &&
              parseText.front()[0] != '-')
        {
            printf("Got option value: '%s'\n", parseText.front().c_str());
            optResult.values.push_back(parseText.front());
            parseText.pop_front();
            paramCounter++; 
        }
    
        return std::optional<OptionResult>(optResult);
    } else {
        parseResult.errors.push_back({-1, optionName});
    }

    return std::nullopt;
}

ParseResult
Parser::parse(int argc, char* argv[]) const
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
    }

    // Check for just options, no command.
    if(args.size() == 0) return result;

    for(const auto& com : mCommands) {
        if(com->name == args[0]) {
            printf("Found command '%s'\n", com->name.c_str());
            result.command = com;

            args.pop_front();
            while(!args.empty() && args.front()[0] == '-') {
                auto optResult = parseOption(com, args, result);
                if(optResult.has_value()) {
                    result.options.push_back(optResult.value());
                }
            }

            // Anything left over is a positional argument.
            while(!args.empty()) {
                printf("Got positional arg: '%s'\n", args.front().c_str());
                result.positionalArgs.push_back(args.front());
                args.pop_front();
            }
            break;
        }
    }

    return result;
}

}
