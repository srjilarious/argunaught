#include "argunaught.hpp"

namespace argunaught
{


Command::Command(
        std::string n, 
        OptionsList opt, 
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
        OptionsList options, 
        CommandHandler func
    )
{
    mCommands.push_back(std::make_shared<Command>(name, options, func));
    return *this;
}

Parser& 
Parser::options(
        OptionsList options
    )
{
    for(auto opt : options) {
        mOptions.push_back(opt);
    }
    return *this;
}

Option parseOption(std::vector<std::string>& parseText)
{
    Option opt;

    return opt;
}

ParseResult
Parser::parse(int argc, char* argv[]) const
{
    ParseResult result;

    // Skip the executable name
    argv = &argv[1];
    argc--;

    // Create a vector of strings
    std::vector<std::string> args;
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
        }
        result.command = com;
    }

    return result;
}

}
