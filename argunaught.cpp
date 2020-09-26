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
        OptionList options
    )
{
    // for(auto opt : options) {
    //     mOptions.addOption(opt);
    // }
    return *this;
}

OptionList::OptionList(std::vector<Option> opts)
{
    for(auto& opt : opts) {
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
    auto it = std::find_if(std::begin(mOptions), std::end(mOptions), [optionName] (const Option& opt) {
        return opt.longName == optionName;
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
        return opt.shortName == optionName;
    });

    if(it != mOptions.end()) {
        return std::optional<Option>(*it);
    }

    return std::nullopt;
}

OptionResult 
Parser::parseOption(std::deque<std::string>& parseText) const
{
    OptionResult optResult;

    // Expect atleast one value in parseText
    // Expect first value begins with '-'

    // Get the option name out.
    std::string optionFullName = parseText[0];
    if(optionFullName.size() == 1) {
        // Error, not option name
    }

    std::string optionName;
    std::optional<Option> opt;

    // Check for a long name
    if(optionFullName[1] == '-') {
        // Skip over '--'
        optionName = optionFullName.substr(2);
        printf("Got long option name: '%s'\n", optionName.c_str());
        opt = mOptions.findLongOption(optionName);
    }
    else {
        // Skip over '-'
        optionName = optionFullName.substr(1);
        printf("Got short option name: '%s'\n", optionName.c_str());
        opt = mOptions.findShortOption(optionName);
    }

    parseText.pop_front();

    if(opt != std::nullopt) {
        Option& foundOption = opt.value();
        optResult.optionName = foundOption.longName;

        int paramCounter = 0;

        // Parse any values until the next option.
        
        while(!parseText.empty() && 
              (foundOption.maxNumParams == -1 || 
               paramCounter < foundOption.maxNumParams) &&
              parseText.front()[0] == '-')
        {
            printf("Got option value: '%s'\n", parseText.front().c_str());
            optResult.values.push_back(parseText.front());
            parseText.pop_front();    
        }
    }

    return optResult;
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
    // while(args[0][0] == '-') {

    // }

    // Check for just options, no command.
    if(args.size() == 0) return result;

    for(const auto& com : mCommands) {
        if(com->name == args[0]) {
            printf("Found command '%s'\n", com->name.c_str());
            result.command = com;

            args.pop_front();
            while(!args.empty()) {
                auto optResult = parseOption(args);
                result.options.push_back(optResult);
            }
            break;
        }
    }

    return result;
}

}
