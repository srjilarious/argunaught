
// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN  
#include "catch2/catch.hpp"

#include <argunaught/argunaught.hpp>

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



TEST_CASE( "Test negative parameters", "[negatives]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Cool Grouped commands")
        .options({
                {"delta", "d", "Another global option", 0},
                {"check", "c", "A cool global option", 1}
            })
        .group("Fancy")
            .command("sub", "Unit test sub-command", 
            [&counter] (auto& parseResult) -> int 
            { 
                counter = 100;
                return 0;
            })
        .endGroup();

    SECTION( "A negative number should work with no command") {
        const char* args[] = {"test", "-123"};
        auto parseResult = argu.parse(2, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 1);
        REQUIRE(parseResult.positionalArgs[0] == "-123");
    }

    SECTION( "An option plus a negative number should work with no command") {
        const char* args[] = {"test", "--delta", "-123"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 1);
        REQUIRE(parseResult.positionalArgs[0] == "-123");
    }

    SECTION( "An option plus a few numbers should work with no command") {
        const char* args[] = {"test", "--delta", "12", "-456", "256"};
        auto parseResult = argu.parse(5, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "delta");
        REQUIRE(parseResult.options[0].values.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 3);
        REQUIRE(parseResult.positionalArgs[0] == "12");
        REQUIRE(parseResult.positionalArgs[1] == "-456");
        REQUIRE(parseResult.positionalArgs[2] == "256");
    }

    SECTION( "An option with a negative number parameter should work") {
        const char* args[] = {"test", "-c", "-456"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "check");
        REQUIRE(parseResult.options[0].values.size() == 1);
        REQUIRE(parseResult.options[0].values[0] == "-456");
        REQUIRE(parseResult.positionalArgs.size() == 0);
    }

    SECTION( "An option with a negative number parameter and positional args should work") {
        const char* args[] = {"test", "-c", "-456", "-100", "-200"};
        auto parseResult = argu.parse(5, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        REQUIRE(parseResult.options[0].optionName == "check");
        REQUIRE(parseResult.options[0].values.size() == 1);
        REQUIRE(parseResult.options[0].values[0] == "-456");
        REQUIRE(parseResult.positionalArgs.size() == 2);
        REQUIRE(parseResult.positionalArgs[0] == "-100");
        REQUIRE(parseResult.positionalArgs[1] == "-200");
    }

    SECTION( "A command with a negative number should work with no command") {
        const char* args[] = {"test", "sub", "-123"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 1);
        REQUIRE(parseResult.positionalArgs[0] == "-123");
    }

    SECTION( "A command with a few numbers plus a negative number should work with no command") {
        const char* args[] = {"test", "sub", "10", "-123", "200"};
        auto parseResult = argu.parse(5, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 0);
        REQUIRE(parseResult.positionalArgs.size() == 3);
        REQUIRE(parseResult.positionalArgs[0] == "10");
        REQUIRE(parseResult.positionalArgs[1] == "-123");
        REQUIRE(parseResult.positionalArgs[2] == "200");
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

