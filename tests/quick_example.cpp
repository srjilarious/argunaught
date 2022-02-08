// Quick Example showing the basic usage of argunaught

#include <argunaught/argunaught.hpp>
#include <stdio.h>

int main(int argc, const char* argv[])
{
    auto args = argunaught::Parser("Cool Test App")
        .options({
            {"gamma", "g", "A global option", 1},
            {"delta", "d", "Another global option", 0}
        })
        .command(
            "version", "Gets the version.", [] (auto& parseResult) -> int 
            { 
                printf("Version: 1\n"); 
                return 0;
            }
        )
        .command(
            "sub", "An awesome sub-command.",
            {
                {"test", "t", "A test option.", 0},
                {"alpha", "a", "cool option", -1},
                {"beta", "b", "cool other option", 2}
            },
            [] (auto& parseResult) -> int {
                return 0;
            } 
        );
    
    auto formatter = argunaught::DefaultHelpFormatter(args);
    auto helpStr = formatter.helpString();
    printf("%s", helpStr.c_str());
    
    auto parseResult = args.parse(argc, argv);

    // Run the associated command
    if(parseResult.hasCommand()) {
        parseResult.runCommand();
    }
}
