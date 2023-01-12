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

ParserConfigErrorType 
OptionList::addOption(Option opt)
{
    if(opt.longName == "") {
        return ParserConfigErrorType::LongOptionNameMissing;
    }

    if(std::isdigit(opt.longName[0])) {
        return ParserConfigErrorType::OptionBeginsWithNumber;
    }

    if(opt.shortName.size() > 0 && std::isdigit(opt.shortName[0])) {
        return ParserConfigErrorType::OptionBeginsWithNumber;
    }

    for(auto& el : mOptions) {
        if(opt.longName == el.longName) {
            return ParserConfigErrorType::DuplicateOption;
        } 
        else if(opt.shortName.size() > 0 &&
                opt.shortName == el.shortName) {
            return ParserConfigErrorType::DuplicateOption;
        }
    }

    mOptions.push_back(opt);
    return ParserConfigErrorType::NoError;
}

ParserConfigErrorType 
OptionList::addOptions(const OptionList& opts)
{
    for(auto& el : opts.mOptions) {
        auto res = addOption(el);
        if(res != ParserConfigErrorType::NoError) {
            // Error out on first issue
            return res;
        }
    }

    return ParserConfigErrorType::NoError;
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
