#include "catch2/catch.hpp"
#include <argunaught/argunaught.hpp>

TEST_CASE( "Test positional args", "[options]" ) {
    auto argu = argunaught::Parser("Cool Test App")
        .options(    
            {{"gamma", "g", "A global option", 0},
             {"alpha", "a", "Takes an arg!", 1}}
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

    SECTION("Should be able to get an OptionResult w/ default value") {
        const char* args[] = {"test", "-a", "arg1", "arg2"};
        auto parseResult = argu.parse(4, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        
        REQUIRE(parseResult.hasOption("alpha") == true);

        auto gammaOpt = parseResult.getOption("alpha", "default");
        
        REQUIRE(gammaOpt.optionName == "alpha");
        REQUIRE(gammaOpt.values.size() == 1);
        REQUIRE(gammaOpt.values[0] == "arg1");

        REQUIRE(parseResult.positionalArgs.size() == 1);
        REQUIRE(parseResult.positionalArgs[0] == "arg2");
        REQUIRE(!parseResult.hasCommand());
    }

    SECTION("Should be able to get an OptionResult w/ default value") {
        const char* args[] = {"test", "-g", "arg1", "arg2"};
        auto parseResult = argu.parse(4, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 1);
        
        REQUIRE(parseResult.hasOption("alpha") == false);

        auto gammaOpt = parseResult.getOption("alpha", "default");
        
        REQUIRE(gammaOpt.optionName == "alpha");
        REQUIRE(gammaOpt.values.size() == 1);
        REQUIRE(gammaOpt.values[0] == "default");

        REQUIRE(parseResult.positionalArgs.size() == 2);
        REQUIRE(parseResult.positionalArgs[0] == "arg1");
        REQUIRE(parseResult.positionalArgs[1] == "arg2");
        REQUIRE(!parseResult.hasCommand());
    }
}
