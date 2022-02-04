#include <argunaught/argunaught.hpp>

namespace argunaught
{

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

}
