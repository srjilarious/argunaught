#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace argunaught
{

// Forward declarations
struct Option;
struct OptionResult;
struct Command;
struct SubParser;
class ParseResult;
class Parser;


//! A collection of options found during parsing.
using OptionResultList = std::vector<OptionResult>;

//! A standard command that runs like a miniature main
using CommandHandler = std::function<int (const ParseResult&)>;

//! A command that is expected to instantiate a parser in its handler to 
//! parse the remaining positional arguments to allow breaking sets of 
//! commands into sub groups.
using SubParserHandler = std::function<ParseResult (const Parser& parent, OptionResultList foundOptions, std::deque<std::string> args)>;

//! Shared pointer to a command object
using CommandPtr = std::shared_ptr<Command>;

//! A list of command object pointers
using CommandList = std::vector<CommandPtr>;

//! Shared pointer to a subparser
using SubParserPtr = std::shared_ptr<SubParser>;

//! A list of subparser pointers
using SubParserList = std::vector<SubParserPtr>;

}
