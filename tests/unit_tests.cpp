
// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_MAIN  
#include "catch2/catch.hpp"

#include <argunaught/argunaught.hpp>

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

        auto gammaOptVal = parseResult.getOption("gamma").value();
        REQUIRE(gammaOptVal.optionName == "gamma");
        REQUIRE(gammaOptVal.values.size() == 0);

        REQUIRE(parseResult.positionalArgs.size() == 2);
        REQUIRE(parseResult.positionalArgs[0] == "arg1");
        REQUIRE(parseResult.positionalArgs[1] == "arg2");
        REQUIRE(!parseResult.hasCommand());
    }
}