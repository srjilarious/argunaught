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
    mCommands.push_back(Command(name, options, func));
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

}
