#pragma once

#include <string>
#include <vector>
#include <functional>

namespace argunaught
{

class parser
{
public:
    parser();

    parser& command(std::string name, std::function<void ()> func);

};
}
