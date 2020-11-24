// Set of tests, blocking out how I want the API to look.

#include <argunaught/argunaught.hpp>
#include <stdio.h>

int main(int argc, char** argv)
{
    auto args = argunaught::Parser("Cool Test App")
        .options({
            {"gamma", "g", "A global option", 1},
            {"delta", "d", "Another global option", 0}
        })
        .command(
            "version", "Gets the version.", [] (auto& parseResult) -> int { printf("Version: 1\n"); }
        )
        .command(
            "sub", "An awesome sub-command.",
            {
                {"test", "t", "A test option.", 0},
                {"alpha", "a", "cool option", -1},
                {"beta", "b", "cool other option", 2}
            },
            [] (auto& parseResult) -> int { /*argunaught::opt("h", "help")*/ } 
        );

    printf("%s", args.help().c_str());
    
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
    if(parseResult.command) {
        parseResult.command->handler(parseResult);
    }

    // TODO:
    // Fix positional args not working for:
    // ./test --gamma sub --yeta --delta 1 2 3 --alpha one two three
}
