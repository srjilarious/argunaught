#include <argunaught/argunaught.hpp>

namespace argunaught
{

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

}
