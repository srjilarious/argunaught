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
    // Check that the option was configured on the parser in the first place.
    auto opt = optionsList.findLongOption(optionLongName);
    if(!opt.has_value()) {
        throw std::runtime_error("Trying to get option that was not configured: '" + optionLongName + "'!");
    }

    // See if we parsed out this option.
    std::optional<OptionResult> result = std::nullopt;
    auto found = std::find_if(options.begin(), options.end(), [&](auto val) {
        return val.optionName == optionLongName;
    });

    if(found != options.end()) {
        result = *found;
    }

    // Return nullopt if not parsed.
    return result;
}

OptionResult 
ParseResult::getOption(
        std::string optionLongName, 
        std::string defaultVal) const
{
    // Check that the option was configured on the parser in the first place.
    auto opt = optionsList.findLongOption(optionLongName);
    if(!opt.has_value()) {
        throw std::runtime_error("Trying to get option that was not configured: '" + optionLongName + "'!");
    }

    // See if we parsed out this option.
    auto found = std::find_if(options.begin(), options.end(), [&](auto val) {
        return val.optionName == optionLongName;
    });

    if(found != options.end()) {
        return *found;
    }

    // Return an OptionResult with the default value.
    return OptionResult{optionLongName, {defaultVal}};
}


bool 
ParseResult::hasOption(std::string optionLongName) const
{
    return getOption(optionLongName) != std::nullopt;
}

}
