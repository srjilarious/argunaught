#include "catch2/catch.hpp"
#include <argunaught/argunaught.hpp>

TEST_CASE( "Test grouped commands", "[groups]" ) {
    int counter = 0;
    auto argu = argunaught::Parser("Cool Grouped commands")
        .options({
                {"delta", "d", "Another global option", 0},
                {"check", "x", "A cool global option", 1}
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
