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
                {"test", "t", "A test option.", 0}
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

    args.parse(argc, argv);
}
