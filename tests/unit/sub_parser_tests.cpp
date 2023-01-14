#include "catch2/catch.hpp"
#include <argunaught/argunaught.hpp>

TEST_CASE( "Test sub parsers", "[subparser]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Cool Test App")
        .options(
            {{"global", "g", "A global option", 0}}
        )
        .command("sub", "Unit test sub-command", 
            { /* No options */ },
            [&counter] (auto& parseResult) -> int 
            {
                return 0;
            })
        .subParser("fancy", "My fancy sub parser",
            {{"test", "t", "An option for fancy"}},
            [&counter] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
            {
                auto subParser = argunaught::Parser("Cool Test App - sub")
                    .options(parser.options())
                    .command("work", "Unit test sub-command", 
                        {},
                        [&] (auto& subParseResult) -> int 
                        {
                            counter = 200;
                            return 0;
                        });

                return subParser.parse(args, optionResults);
            }
        );
    
    SECTION( "Single option with param should work") {
        std::deque<std::string> args = {"-g"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.positionalArgs.size() == 0);
        REQUIRE(!parseResult.hasCommand());
        REQUIRE(counter == 0);
    }
    
    SECTION( "Should be able to call sub parser's command") {
        std::deque<std::string> args = {"fancy", "work", "-g"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.positionalArgs.size() == 0);

        REQUIRE(counter == 0);
        REQUIRE(parseResult.hasCommand());
        parseResult.runCommand();
        REQUIRE(counter == 200);
    }
}

TEST_CASE( "Test sub-command parsers", "[sub-commands]" ) {
    int counter = 0;
    std::string help = "";
    auto argu = argunaught::Parser("Cool Test App")
        .options(
            {{"global", "g", "A global option", 0}}
        )
        .command("sub", "Unit test sub-command", 
            {
                {"com1", "c", "A command option", 0},
                {"com2", "", "Another command option", 2},
            },
            [&counter, &help] (auto& parseResult) -> int 
            { 
                // Build up a new parser for the sub command with separate options,
                // but inheriting the parent parser options list.
                // TODO: implement.
                argunaught::ParserPtr subParser = std::make_shared<argunaught::Parser>("Cool Test App - sub");
                    subParser->options(
                        {{"test", "t", "A sub parser global option", 0}}
                    )
                    .command("work", "Unit test sub-command", 
                        {},
                        [subParser, &counter, &help] (auto& subParseResult) -> int 
                        {
                            counter = 200;
                            return 0;
                        })
                    .command("help", "Print help for sub-command", 
                        {},
                        [subParser, &counter, &help] (auto& subParseResult) -> int
                        {
                            counter = 300;
                            argunaught::DefaultFormatStyle style{};
                            auto helpFormatter = argunaught::DefaultHelpFormatter(*subParser, style, true);
                            help = helpFormatter.helpString();
                            return 0;
                        });

                auto subResult = subParser->parse(parseResult);
                subResult.runCommand();
                return 0;
            });

    SECTION( "Single option with param should work") {
        std::deque<std::string> args = {"-g"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.positionalArgs.size() == 0);
        REQUIRE(!parseResult.hasCommand());
        REQUIRE(counter == 0);
    }

    SECTION( "Single option with param should work") {
        std::deque<std::string> args = {"sub", "-g", "work", "--test"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        
        // Only the global option is found at this point
        REQUIRE(parseResult.options.size() == 1);

        // The remaining sub command arguments are left as positional args.
        REQUIRE(parseResult.positionalArgs.size() == 2);
        REQUIRE(parseResult.hasCommand());
        REQUIRE(counter == 0);
        
        // This will run the command which runs its own sub parser.
        // The work sub command will set the counter variable to 200.
        parseResult.runCommand();
        REQUIRE(counter == 200);
    }

    SECTION("Should be able to generate sub parser help") {
        std::deque<std::string> args = {"sub", "help"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.hasCommand());

        // This will run the command which runs its own sub parser.
        // The work sub command will set the counter variable to 200.
        parseResult.runCommand();
        REQUIRE(counter == 300);
        std::string expectedHelp = 
R"(Cool Test App - sub

Global Options:
    --test, -t           - A sub parser global option
    --global, -g         - A global option
    --com1, -c           - A command option
    --com2               - Another command option

Commands:
    work                 - Unit test sub-command
    help                 - Print help for sub-command

)";
        REQUIRE(help == expectedHelp);
    }

}
