#include <argunaught/argunaught.hpp>

namespace argunaught
{

SubParser::SubParser(
        std::string n, 
        std::string h, 
        std::vector<Option> opt, 
        SubParserHandler f
    )
    : name(n), help(h), options(opt), handler(f)
{
}

}
