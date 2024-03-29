#include <argunaught/argunaught.hpp>

namespace argunaught
{

namespace
{

std::string
getParserConfigErrorName(ParserConfigErrorType type)
{
    switch(type)
    {
        case ParserConfigErrorType::LongOptionNameMissing:
            return "LongOptionNameMissing";

        case ParserConfigErrorType::OptionBeginsWithNumber:
            return "OptionBeginsWithNumber";

        case ParserConfigErrorType::DuplicateOption:
            return "DuplicateOption";

        case ParserConfigErrorType::CommandNameMissing:
            return "CommandNameMissing";

        case ParserConfigErrorType::DuplicateCommandName:
            return "DuplicateCommandName";
        
        case ParserConfigErrorType::CommandGroupNameMissing:
            return "CommandGroupNameMissing";

        default:
            return "UnknownError";
    }
}

}


Parser::Parser(std::string name, std::string banner)
    : mName(name), mBanner(banner)
{
}


Parser& 
Parser::description(std::string d) 
{ 
    mDescription = d;
    return *this;
}

Parser& 
Parser::usage(std::string u) 
{ 
    mUsage = u;
    return *this;
}

Parser& 
Parser::subParser(
        std::string name, 
        std::string help, 
        SubParserHandler func)
{
    return subParser(name, help, {}, func);
}

Parser& 
Parser::subParser(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        SubParserHandler func
    )
{
    // Check for a missing command name.
    if(name == "") {
        auto err = ParserConfigErrorType::CommandNameMissing;
        mConfigErrors.push_back({
            err,
            "Error adding subparser [" + getParserConfigErrorName(err) + "]:"
            " description='" + help + "'"
        });

        return *this;
    }
    
    // Check for a duplicate command name
    auto exists = checkCommandNameExists(name);
    if(exists) {
        auto err = ParserConfigErrorType::DuplicateCommandName;
        mConfigErrors.push_back({
            err,
            "Error adding subparser [" + getParserConfigErrorName(err) + "]:"
            " name='" + name + "',"
            " description='" + help + "'"
        });

        return *this;
    }

    mSubParsers.push_back(std::shared_ptr<SubParser>(new SubParser(name, help, options, func)));
    return *this; 
}

Parser& 
Parser::command(
        std::string name, 
        std::string help, 
        CommandHandler func)//,
        // bool handlesSubParsers)
{
    return command(name, help, {}, func);
}

Parser& 
Parser::command(
        std::string name, 
        std::string help, 
        std::vector<Option> options, 
        CommandHandler func
    )
{
    // Check for a missing command name.
    if(name == "") {
        auto err = ParserConfigErrorType::CommandNameMissing;
        mConfigErrors.push_back({
            err,
            "Error adding command [" + getParserConfigErrorName(err) + "]:"
            " description='" + help + "'"
        });

        return *this;
    }
    
    // Check for a duplicate command name
    auto exists = checkCommandNameExists(name);
    if(exists) {
        auto err = ParserConfigErrorType::DuplicateCommandName;
        mConfigErrors.push_back({
            err,
            "Error adding command [" + getParserConfigErrorName(err) + "]:"
            " name='" + name + "',"
            " description='" + help + "'"
        });

        return *this;
    }

    mCommands.push_back(std::make_shared<Command>(name, help, options, func));
    return *this;
}

Parser& 
Parser::options(
        std::vector<Option> options
    )
{
    for(auto opt : options) {
        auto res = mOptions.addOption(opt);
        if(res != ParserConfigErrorType::NoError) {
            mConfigErrors.push_back({
                res,
                "Error adding option [" + getParserConfigErrorName(res) + "]:"
                " '--" + opt.longName + "',"
                " '-" + opt.shortName + "'," 
                " numParams=" + std::to_string(opt.maxNumParams) + ","
                " description='" + opt.description + "'"
            });
        }
    }
    return *this;
}

Parser& 
Parser::options(
        const OptionList& opts
    )
{
    return options(opts.values());
}

CommandGroup& 
Parser::group(std::string name)
{
    mGroups.push_back(CommandGroup(this, name));
    return *(mGroups.end()-1);
}

CommandGroup& 
Parser::group(std::string name, std::string description)
{
    mGroups.push_back(CommandGroup(this, name, description));
    return *(mGroups.end()-1);
}


bool 
Parser::checkCommandNameExists(std::string name) const
{
    // First check if the name exists in the command list.
    auto comit = std::find_if(mCommands.begin(), mCommands.end(), [&name] (auto& el) {
        return el->name == name;
    });

    if(comit != mCommands.end()) {
        return true;
    }

    // Also need to check sub parsers which are just fancy commands.
    auto spit = std::find_if(mSubParsers.begin(), mSubParsers.end(), [&name] (auto& el) {
        return el->name == name;
    });

    if(spit != mSubParsers.end()) {
        return true;
    }

    return false;
}


std::optional<OptionResult>
Parser::parseOption(std::shared_ptr<Command> command, 
                    std::deque<std::string>& parseText,
                    ParseResult& parseResult) const
{
    OptionResult optResult;

    // Expect atleast one value in parseText
    if(parseText.size() == 0) return std::nullopt;

    // Get the option name out.
    std::string optionFullName = parseText[0];

    // Make sure the option is not just a - or --
    if(optionFullName == "-" || 
       optionFullName == "--") 
    {
           parseText.pop_front();
           parseResult.currItemPos++;

           return std::nullopt;
    }

    std::string optionName;
    std::optional<Option> opt;

    // Check for a long name
    if(optionFullName[0] == '-' && optionFullName[1] == '-') {
        // Skip over '--'
        optionName = optionFullName.substr(2);
        ARGUNAUGHT_TRACE("Got long option name: '%s'\n", optionName.c_str());
        if(command != nullptr) {
            opt = command->options.findLongOption(optionName);
        }

        if(!opt.has_value()) {
            ARGUNAUGHT_TRACE("No command option, checking for global option.\n");
            opt = mOptions.findLongOption(optionName);
        }
    }
    else if(optionFullName[0] == '-') {
        // Skip over '-'
        optionName = optionFullName.substr(1);
        ARGUNAUGHT_TRACE("Got short option name: '%s'\n", optionName.c_str());

        if(command != nullptr) {
            opt = command->options.findShortOption(optionName);
        }

        if(!opt.has_value()) {
            ARGUNAUGHT_TRACE("No command option, checking for global option.\n");
            opt = mOptions.findShortOption(optionName);
        }
    }
    else {
        return std::nullopt;
    }

    parseText.pop_front();
    parseResult.currItemPos++;

    ARGUNAUGHT_TRACE("Option found: %s\n", opt.has_value() ? "True" : "False");
    if(opt.has_value()) {
        Option& foundOption = opt.value();
        optResult.optionName = foundOption.longName;

        int paramCounter = 0;

        ARGUNAUGHT_TRACE("Checking for option values.\n");

        // Parse any values until the next option.
        while(!parseText.empty() && 
              (foundOption.maxNumParams == -1 || 
               paramCounter < foundOption.maxNumParams)
              )
        {
            const auto& currOptValue = parseText.front();
            if(currOptValue[0] == '-' && currOptValue.size() > 1) {
                // Only break out of the option parameter loop if
                // we find a `-` not followed by a number.  negative
                // numbers are fine.
                if(!std::isdigit(currOptValue[1])) break;
            }

            ARGUNAUGHT_TRACE("Got option value: '%s'\n", parseText.front().c_str());
            optResult.values.push_back(parseText.front());
            parseText.pop_front();
            parseResult.currItemPos++;

            paramCounter++; 
        }
    
        ARGUNAUGHT_TRACE("Done checking for option values. %d found\n", paramCounter);
        return std::optional<OptionResult>(optResult);
    } else {
        parseResult.errors.push_back({
                ParseErrorType::UnknownOption, 
                static_cast<int>(parseResult.currItemPos),
                optionName
            });
    }

    return std::nullopt;
}

CommandPtr 
Parser::getCommand(std::string name) const
{
    auto it = std::find_if(mCommands.begin(), mCommands.end(), [&name] (auto& c) {
        return c->name == name;
    });

    if(it != mCommands.end()) {
        return *it;
    }

    // Also look for the command in command groups
    for(const auto& g : mGroups) {
        auto cmd = g.getCommand(name);
        if(cmd) {
            return cmd;
        }
    }

    return nullptr;
}

ParseResult
Parser::parse(int argc, const char* argv[]) const
{
    // Skip the executable name
    argv = &argv[1];
    argc--;

    // Create a deque of strings
    std::deque<std::string> args;
    for(int ii = 0; ii < argc; ++ii)
    {
        args.emplace_back(argv[ii]);
    }

    return parse(args);
}


ParseResult
Parser::parse(ParseResult const& prevParseResult)
{
    // Create a deque of strings
    std::deque<std::string> args;
    for(auto& arg : prevParseResult.positionalArgs)
    {
        args.emplace_back(arg);
    }

    mOptions.addOptions(prevParseResult.optionsList.values());
    return parse(args);
}

ParseResult
Parser::parse(std::deque<std::string> args, OptionResultList existingOptions) const
{
    ParseResult result;

    for(const auto& opt : existingOptions) {
        result.options.push_back(opt);
        result.currItemPos++;
    }

    result.optionsList.addOptions(mOptions);

    if(args.size() == 0) return result;

    // parse any options before the command as global options
    while(!args.empty() && args.front()[0] == '-') {
        const auto& currArg = args.front();
        if(currArg.size() > 1 && std::isdigit(currArg[1])) {
            // If we found a negative number, then we must have found 
            // the beginning of the positional arguments, so break.
            break;
        }

        auto optResult = parseOption(nullptr, args, result);
        if(optResult.has_value()) {
            result.options.push_back(optResult.value());
        }
        else {
            break;
        }
    }

    // Check for just options, no command.
    if(args.size() == 0) return result;

    // Create a combined list of un-grouped commands and grouped commands
    CommandList allCommands;
    std::copy(mCommands.begin(), mCommands.end(), std::back_inserter(allCommands));
    for(const auto& group : mGroups) {
        std::copy(
            group.commands.begin(), 
            group.commands.end(), 
            std::back_inserter(allCommands)
        );
    }

    for(const auto& com : allCommands) {
        if(com->name == args[0]) {
            ARGUNAUGHT_TRACE("Found command '%s'\n", com->name.c_str());
            result.command = com;

            args.pop_front();
            result.currItemPos++;

            result.optionsList.addOptions(com->options);

            while(!args.empty() && args.front()[0] == '-') {
                const auto& currArg = args.front();
                if(currArg.size() > 1 && std::isdigit(currArg[1])) {
                    // If we found a negative number, then we must have found 
                    // the beginning of the positional arguments, so break.
                    break;
                }

                auto optResult = parseOption(com, args, result);
                if(optResult.has_value()) {
                    result.options.push_back(optResult.value());
                }
                else {
                    break;
                }

                ARGUNAUGHT_TRACE("Done parsing option '%s', ended with %lu params\n",
                optResult.value().optionName.c_str(), optResult.value().values.size());
            }

            break;
        }
    }
    
    // Create a combined list of un-grouped commands and grouped commands
    SubParserList allSubParsers;
    std::copy(mSubParsers.begin(), mSubParsers.end(), std::back_inserter(allSubParsers));
    for(const auto& group : mGroups) {
        std::copy(
            group.subParsers.begin(), 
            group.subParsers.end(), 
            std::back_inserter(allSubParsers)
        );
    }

    for(const auto& subCom : allSubParsers) {
        if(subCom->name == args[0]) {
            ARGUNAUGHT_TRACE("Found sub command '%s'\n", subCom->name.c_str());
            
            args.pop_front();
            result.currItemPos++;
            result = subCom->handler(*this, result.options, args);
            
            return result;
        }
    }

    // Fall through in case of no command, sub command, or remaining args for command.
    ARGUNAUGHT_TRACE("Checking positional args, %lu left", args.size());

    // Anything left over is a positional argument.
    while(!args.empty()) {
        ARGUNAUGHT_TRACE("Got positional arg: '%s'\n", args.front().c_str());
        result.positionalArgs.push_back(args.front());
        args.pop_front();
        result.currItemPos++;
    }
    return result;
}

}
