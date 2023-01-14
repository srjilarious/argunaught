#include "catch2/catch.hpp"
#include <argunaught/argunaught.hpp>

TEST_CASE( "Test commands", "[command]" ) {
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

