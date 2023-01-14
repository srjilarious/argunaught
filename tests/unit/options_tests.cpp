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

    SECTION( "A short option must not begin with a number" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"test", "1", "A global option", 1}
            });

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::OptionBeginsWithNumber);
        auto expectedMessage = 
                "Error adding option [OptionBeginsWithNumber]: "
                "'--test', '-1', numParams=1, description='A global option'";
        REQUIRE(errors[0].message == expectedMessage);
    }

    SECTION( "A long option must not begin with a number" ) {
        auto argu = argunaught::Parser("Cool Test App")
            .options({
                {"1test", "t", "A global option", 1}
            });

        // TODO: Add error checks to parser itself?
        REQUIRE(argu.hasConfigurationError());
        auto errors = argu.parserConfigErrors();
        REQUIRE(errors.size() == 1);
        REQUIRE(errors[0].type == argunaught::ParserConfigErrorType::OptionBeginsWithNumber);
        auto expectedMessage = 
                "Error adding option [OptionBeginsWithNumber]: "
                "'--1test', '-t', numParams=1, description='A global option'";
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

TEST_CASE( "Test global options only", "[options]" ) {
    auto argu = argunaught::Parser("Cool Test App")
        .options({
            {"gamma", "g", "A global option", 1},
            {"delta", "d", "Another global option", 0},
            {"alpha", "", "A long-only option", 0},
            {"beta", "", "Another long-only option", 0}
        });
    
    SECTION("Multiple long only options should work") {
        const char* args[] = {"test", "--alpha", "--beta"};
        auto parseResult = argu.parse(3, args);
        REQUIRE(!parseResult.hasError());
        REQUIRE(parseResult.options.size() == 2);
        REQUIRE(parseResult.hasOption("alpha"));
        REQUIRE(parseResult.options[0].optionName == "alpha");
        REQUIRE(parseResult.options[0].values.size() == 0);

        REQUIRE(parseResult.hasOption("beta"));
        REQUIRE(parseResult.options[1].optionName == "beta");
        REQUIRE(parseResult.options[1].values.size() == 0);
    }

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
