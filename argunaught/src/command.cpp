#include <argunaught/argunaught.hpp>

namespace argunaught
{

Command::Command(
        std::string n, 
        std::string h, 
        std::vector<Option> opt, 
        CommandHandler f//, 
        // bool _handlesSubParsers
    )
    : name(n), description(h), options(opt), handler(f)//, handlesSubParsers(_handlesSubParsers)
{
}

}
