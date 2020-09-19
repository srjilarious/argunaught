#include "argunaught.hpp"

namespace argunaught
{

parser::parser()
{

}

parser& 
parser::command(
        std::string name, 
        std::function<void ()> func)
{
    return *this;    
}

}
