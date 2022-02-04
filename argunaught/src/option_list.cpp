#include <argunaught/argunaught.hpp>

namespace argunaught
{

OptionList::OptionList(std::vector<Option> opts)
{
    for(auto& opt : opts) {
        ARGUNAUGHT_TRACE("Adding option -%s, --%s\n", opt.shortName.c_str(), opt.longName.c_str());
        addOption(opt);
    }
}

OptionList::OptionList(const OptionList& opts)
    : OptionList(opts.mOptions)
{
}

OptionError 
OptionList::addOption(Option opt)
{
    // FIX: Currently no duplicate checks.
    mOptions.push_back(opt);
    return OptionError::None;
}

OptionError 
OptionList::addOptions(const OptionList& opts)
{
    // FIX: Currently no duplicate checks.
    mOptions.insert(mOptions.end(), opts.mOptions.begin(), opts.mOptions.end());
    return OptionError::None;
}

std::optional<Option> 
OptionList::findShortOption(std::string optionName) const
{
    ARGUNAUGHT_TRACE("Looking for short option, have %d options\n", mOptions.size());
    auto it = std::find_if(std::begin(mOptions), std::end(mOptions), [optionName] (const Option& opt) {
        ARGUNAUGHT_TRACE("Checking '%s' against '%s'\n", optionName.c_str(), opt.shortName.c_str());
        return opt.shortName == optionName;
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
        return opt.longName == optionName;
    });

    if(it != mOptions.end()) {
        ARGUNAUGHT_TRACE("Found long option in parser.");
        return std::optional<Option>(*it);
    }

    return std::nullopt;
}

}
