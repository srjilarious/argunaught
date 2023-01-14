#include "catch2/catch.hpp"
#include <argunaught/argunaught.hpp>

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
