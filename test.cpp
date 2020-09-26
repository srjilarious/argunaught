// Set of tests, blocking out how I want the API to look.

#include <argunaught.hpp>
#include <stdio.h>

int main(int argc, char** argv)
{
    auto args = argunaught::Parser()
        .command(
            "version", [] (auto& parseResult) -> int { printf("Version: 1\n"); }
        )
        .command(
            "sub", 
            {
                {"test", "t", "A test option.", 0},
                {"alpha", "a", "cool option", -1},
                {"beta", "b", "cool other option", 2}
            },
            [] (auto& parseResult) -> int { /*argunaught::opt("h", "help")*/ } 
        );

    printf("Commands:\n");
    for(auto& command : args.commands()) 
    {
        printf("'%s':\n", command->name.c_str());
        for(auto& option : command->options.values()) {
            printf("    '%s', '%s' - %s\n", 
                option.shortName.c_str(), 
                option.longName.c_str(), 
                option.description.c_str());
        }
        printf("\n");
    }
    
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
}
