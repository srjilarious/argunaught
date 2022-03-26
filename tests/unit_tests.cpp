
// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN  
#include "catch2/catch.hpp"

#include <argunaught/argunaught.hpp>

TEST_CASE( "Test various option conditions", "[options]" ) {
    SECTION( "An option must have long name" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"", "g", "A global option", 1},
                {"delta", "d", "Another global option", 0}
            });

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::LongOptionNameMissing);
        auto expectedMessage = 
                "Error adding option [LongOptionNameMissing]: "
                "'--', '-g', numParams=1, description='A global option'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "An option must be known." ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"global", "g", "A global option", 1},
                {"delta", "d", "Another global option", 0}
            });

        REQUIRE(!argu.hasConfigurationError());
        const char* args[] = {"test", "--beta"};
        auto parseResult = argu.parse(2, args);
        REQUIRE(parseResult.hasError());
        auto errors = parseResult.errors;
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParseErrorType::UnknownOption);
        REQUIRE(errors[0].pos == 1);
        REQUIRE(errors[0].value == "beta");
    }

    SECTION( "An option must be known 2." ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"global", "g", "A global option", 1},
                {"delta", "d", "Another global option", 0}
            });

        REQUIRE(!argu.hasConfigurationError());
        const char* args[] = {"test", "--delta", "--beta"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(parseResult.hasError());
        auto errors = parseResult.errors;
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParseErrorType::UnknownOption);
        REQUIRE(errors[0].pos == 2);
        REQUIRE(errors[0].value == "beta");
    }
}

TEST_CASE( "Test various command conditions", "[command]" ) {
    SECTION( "A command must have a name" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"delta", "d", "Another global option", 0}
            })
            .command("", "Unit test sub-command", 
                { /* No options */ },
                [] (auto& parseResult) -> int 
                {
                    return 0;
                }
            );
            

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::CommandNameMissing);
        auto expectedMessage = 
                "Error adding command [CommandNameMissing]: "
                "description='Unit test sub-command'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "A command can't be a duplicate" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"delta", "d", "Another global option", 0}
            })
            .command("sub", "Unit test sub-command", 
                { },
                [] (auto& parseResult) -> int 
                {
                    return 0;
                }
            )
            .command("sub", "A duplicate unit test sub-command", 
                { },
                [] (auto& parseResult) -> int 
                {
                    return 1;
                }
            );
            

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::DuplicateCommandName);
        auto expectedMessage = 
                "Error adding command [DuplicateCommandName]: "
                "name='sub', "
                "description='A duplicate unit test sub-command'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "A command can't be a duplicate with subarsers" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"delta", "d", "Another global option", 0}
            })
            .subParser("sub", "My fancy sub parser",
                [] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
                {
                    return argunaught::ParseResult{};
                }
            )
            .command("sub", "Unit test sub-command clashes with subparser!", 
                { },
                [] (auto& parseResult) -> int 
                {
                    return 0;
                }
            );
            

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::DuplicateCommandName);
        auto expectedMessage = 
                "Error adding command [DuplicateCommandName]: "
                "name='sub', "
                "description='Unit test sub-command clashes with subparser!'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "A subparser can't be a duplicate" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"delta", "d", "Another global option", 0}
            })
            .subParser("sub", "My fancy sub parser",
                [] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
                {
                    return argunaught::ParseResult{};
                }
            )
            .subParser("sub", "A duplicate unit test subparser",
                [] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
                {
                    return argunaught::ParseResult{};
                }
            );
            

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::DuplicateCommandName);
        auto expectedMessage = 
                "Error adding subparser [DuplicateCommandName]: "
                "name='sub', "
                "description='A duplicate unit test subparser'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "A subparser can't be a duplicate with commands" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"delta", "d", "Another global option", 0}
            })
            .command("sub", "Unit test sub-command clashes with subparser!", 
                { },
                [] (auto& parseResult) -> int 
                {
                    return 0;
                }
            )
            .subParser("sub", "My fancy subparser",
                [] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
                {
                    return argunaught::ParseResult{};
                }
            );
            

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::DuplicateCommandName);
        auto expectedMessage = 
                "Error adding subparser [DuplicateCommandName]: "
                "name='sub', "
                "description='My fancy subparser'";
        REQUIRE(errors[0].message == expectedMessage);
    }
}


TEST_CASE( "Test global options only", "[options]" ) {
    auto argu = argunaught::Parser("Cool Test App")
        .options({
            {"gamma", "g", "A global option", 1},
            {"delta", "d", "Another global option", 0}
        });
    
    SECTION( "Single option without param should work") {
        const char* args[] = {"test", "-d"};
        auto parseResult = argu.parse(2, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
    }

    SECTION( "Single option with param should work") {
        const char* args[] = {"test", "--gamma", "one"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "gamma");
        REQUIRE(parseResult.options[0].values.size() == 1);
        REQUIRE(parseResult.options[0].values[0] == "one");
    }
}

TEST_CASE( "Test sub-commands", "[options]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Cool Test App")
        .command("sub", "Unit test sub-command", 
            [&counter] (auto& parseResult) -> int 
            { 
                counter = 100;
                return 0;
            });

    SECTION( "Single option with param should work") {
        const char* args[] = {"test", "sub"};
        auto parseResult = argu.parse(2, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 0);
        REQUIRE(parseResult.hasCommand());
        REQUIRE(parseResult.command->name == "sub");

        parseResult.runCommand();
        REQUIRE(counter == 100);
    }
}

TEST_CASE( "Test positional args", "[options]" ) {
    auto argu = argunaught::Parser("Cool Test App")
        .options(    
            {{"gamma", "g", "A global option", 0}}
        );

    SECTION( "Just positional args should work") {
        const char* args[] = {"test", "arg1", "arg2", "arg3"};
        auto parseResult = argu.parse(4, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 3);
        REQUIRE(parseResult.positionalArgs[0] == "arg1");
        REQUIRE(parseResult.positionalArgs[1] == "arg2");
        REQUIRE(parseResult.positionalArgs[2] == "arg3");
        REQUIRE(!parseResult.hasCommand());
    }

    SECTION( "Single option with positional args should work") {
        const char* args[] = {"test", "-g", "arg1", "arg2"};
        auto parseResult = argu.parse(4, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        
        REQUIRE(parseResult.hasOption("gamma") == true);

        auto gammaOpt = parseResult.getOption("gamma");
        REQUIRE(gammaOpt != std::nullopt);

        auto gammaOptVal = gammaOpt.value();
        REQUIRE(gammaOptVal.optionName == "gamma");
        REQUIRE(gammaOptVal.values.size() == 0);

        REQUIRE(parseResult.positionalArgs.size() == 2);
        REQUIRE(parseResult.positionalArgs[0] == "arg1");
        REQUIRE(parseResult.positionalArgs[1] == "arg2");
        REQUIRE(!parseResult.hasCommand());
    }
}


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

TEST_CASE( "Test help without global options", "[help]" ) {

    auto argu = argunaught::Parser("Cool Test App")
        .command("sub", "Unit test sub-command", 
            {
                {"com1", "c", "A command option", 0},
                {"com2", "", "Another command option", 2},
            },
            [] (auto& args) -> int { return 0; });

    SECTION("Should be able to generate help without global options") {
        
        argunaught::DefaultFormatStyle style{};
        auto helpFormatter = argunaught::DefaultHelpFormatter(argu, style, true);
        auto help = helpFormatter.helpString();
        std::string expectedHelp = 
R"(Cool Test App

Commands:
    sub                  - Unit test sub-command
      --com1, -c         - A command option
      --com2             - Another command option

)";
        REQUIRE(help == expectedHelp);
    }
}


TEST_CASE( "Test grouped commands", "[groups]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Cool Grouped commands")
        .options({
                {"delta", "d", "Another global option", 0}
            })
        .group("Fancy")
            .command("sub", "Unit test sub-command", 
            [&counter] (auto& parseResult) -> int 
            { 
                counter = 100;
                return 0;
            })
        .endGroup();
    
    SECTION( "Single option without param should work") {
        const char* args[] = {"test", "-d"};
        auto parseResult = argu.parse(2, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
    }

    SECTION( "Single option with param should work") {
        const char* args[] = {"test", "--delta", "sub"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
        REQUIRE(parseResult.hasCommand());
        REQUIRE(parseResult.command->name == "sub");
    }
}

TEST_CASE( "Test parsing via deque", "[deque]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Deque parsed")
        .options({
                {"delta", "d", "Another global option", 0}
            })
        .group("Fancy")
            .command("sub", "Unit test sub-command", 
            [&counter] (auto& parseResult) -> int 
            { 
                counter = 100;
                return 0;
            })
        .endGroup();
    
    SECTION( "Single option without param should work") {
        std::deque<std::string> args = {"-d"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
    }

    SECTION( "Single option with param should work") {
        std::deque<std::string> args = {"--delta", "sub"};
        auto parseResult = argu.parse(args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
        REQUIRE(parseResult.hasCommand());
        REQUIRE(parseResult.command->name == "sub");
    }
}

TEST_CASE( "Test word wrap and formatting helper.", "[utils]" ) {

    SECTION("No wrapping needed should append the original string.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 0, 2048, "Hello World");
        REQUIRE(lineLen == 11);
        REQUIRE(result == "Hello World");
    }

    SECTION("Simple wrapping check.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 0, 8, "Hello World");
        REQUIRE(lineLen == 5);
        REQUIRE(result == "Hello\nWorld");
    }
    
    SECTION("Simple embedded new line.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 0, 8, "Hello\nWorld");
        REQUIRE(lineLen == 5);
        REQUIRE(result == "Hello\nWorld");
    }
    
    SECTION("Word wrap w/ embedded new line.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 0, 8, "Hello World\nBam!");
        REQUIRE(lineLen == 4);
        REQUIRE(result == "Hello\nWorld\nBam!");
    }

    SECTION("Word wrap w/ embedded new line and indentation.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 2, 8, "Hello World\nBam!");
        REQUIRE(lineLen == 6);
        REQUIRE(result == "Hello\n  World\n  Bam!");
    }

    SECTION("If a word is too long, should get hyphenated.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 2, 8, "HelloWorld!");
        REQUIRE(result == "HelloWo-\n  rld!");
        REQUIRE(lineLen == 6);
    }

    SECTION("If a word is too long, should get hyphenated 2.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 2, 8, "HelloWorldBam!");
        REQUIRE(result == "HelloWo-\n  rldBa-\n  m!");
        REQUIRE(lineLen == 4);
    }

    SECTION("If a text has a word that is too long, it should get hyphenated.") {
        std::string result;
        auto lineLen = argunaught::formatAndAppendText(result, 0, 0, 20, 
            "Hello World "
            "It_will_also_hyphenate_a_line_without_spaces_and_"
            "perform_sane_wrapping_on_that really long word."
            " and recover!");
        REQUIRE(result == 
            "Hello World\n"
            "It_will_also_hyphen-\n"
            "ate_a_line_without_-\n"
            "spaces_and_perform_-\n"
            "sane_wrapping_on_th-\n"
            "at really long word.\n"
            "and recover!");
        REQUIRE(lineLen == 12);
    }
}