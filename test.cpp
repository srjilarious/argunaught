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
            "sub", [] (auto& parseResult) -> int { /*argunaught::opt("h", "help")*/ } 
        );
}
