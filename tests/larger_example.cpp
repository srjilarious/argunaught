// A larger example, showing options, commands, subparsers and command groups.

#include <argunaught/argunaught.hpp>
#include <stdio.h>

int main(int argc, const char* argv[])
{
    auto args = argunaught::Parser("Cool Test App", "-=# Amazing app Version 1.0 #=-")
        .options({
            {"gamma", "g", "A global option", 1},
            {"delta", "d", "Another global option\nAnd the description\nspans many\nlines....", 0}
        })
        .command(
            "version", "Gets the version.\nUseful for knowing the version of the app.", [] (auto& parseResult) -> int 
            { 
                printf("Version: 1\n"); 
                return 0;
            }
        )
        .subParser("fancy", "My fancy sub parser",
            {{"test", "t", "An option for fancy"}},
            [&] (const auto& parser, auto optionResults, auto args) -> argunaught::ParseResult
            {
                argunaught::ParserPtr subParser = std::make_shared<argunaught::Parser>("Cool Test App - fancy");

                subParser->options(parser.options())
                    .command("work", "sub parser command!", 
                        {},
                        [&] (auto& subParseResult) -> int 
                        {
                            printf("Did some work!\n");
                            return 0;
                        })
                    .command("other", "sub parser other command!", 
                        {},
                        [&] (auto& subParseResult) -> int 
                        {
                            printf("Did some different work!\n");
                            return 0;
                        })
                    .command("help", "Print fancy's help", 
                        {},
                        // Note we capture the shared pointer by value!
                        [subParser] (auto& subParseResult) -> int 
                        {
                            auto helpFormatter = argunaught::DefaultHelpFormatter(*subParser);
                            auto subHelp = helpFormatter.helpString();
                            printf("%s", subHelp.c_str());
                            return 0;
                        });

                auto result = subParser->parse(args, optionResults);

                // Set a default command if none was chosen. `help` in this case.
                if(!result.hasCommand()) {
                    result.command = subParser->getCommand("help");
                }

                return result;
            }
        )
        .group("Transformative", "Magical state changing commands")
            .command("transmogrify", "Changes matter's state",
                [] (auto& parseResult) -> int 
                {
                    printf("** Transmogrify!\n");
                    return 0;
                })
            .command("evocate", "Magical stuff!",
                {
                    {"fire", "f", "fiery option", 0},
                    {"ice", "i", "cold option", 0},
                    {"long", "l",
                    "Here is a very long description for an option "
                    "that should end up spanning multiple lines to show "
                    "how word wrapping operates when the display width "
                    "is smaller than the amount of space needed to show "
                    "the description.  Embedding a '\\n' is possible to "
                    "make line breaks, but it won't wrap properly to the "
                    "width of the display the way that word wrapping can."
                    "It_will_also_hyphenate_a_line_without_spaces_and_"
                    "perform_sane_wrapping_on_that really long word.", 0}
                },
                [] (auto& parseResult) -> int 
                {
                    printf("** Evoke");
                    if(parseResult.hasOption("fire")) {
                        printf(" FIRE");
                    }
                    if(parseResult.hasOption("ice")) {
                        printf(" ICE");
                    }

                    printf("!\n");
                    return 0;
                })
        .endGroup();

    auto formatter = argunaught::DefaultHelpFormatter(args);
    auto helpStr = formatter.helpString();
    printf("%s", helpStr.c_str());
    
    auto parseResult = args.parse(argc, argv);

    if(parseResult.command) {
        printf("Command: %s\n", parseResult.command->name.c_str());
    }

    printf("Options seen:\n");
    for(int ii = 0; ii < parseResult.options.size(); ii++) {
        auto& opt = parseResult.options[ii];
        printf("    - %s:\n", opt.optionName.c_str());
        for(int jj = 0; jj < opt.values.size(); jj++) {
            printf("        - %s\n", opt.values[jj].c_str());
        }
    }

    printf("Positional Args:\n");
    for(int ii = 0; ii < parseResult.positionalArgs.size(); ii++) {
        printf("    - %s\n", parseResult.positionalArgs[ii].c_str());
    }

    printf("Errors:\n");
    for(int ii = 0; ii < parseResult.errors.size(); ii++) {
        printf("    - %s\n", parseResult.errors[ii].value.c_str());
    }

    // Run the associated command
    if(parseResult.hasCommand()) {
        parseResult.runCommand();
    }

    // TODO:
    // Fix positional args not working for:
    // ./test --gamma sub --yeta --delta 1 2 3 --alpha one two three
}
