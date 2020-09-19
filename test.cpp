// Set of tests, blocking out how I want the API to look.

#include <argunaught.hpp>
#include <stdio.h>

int main(int argc, char** argv)
{
    auto args = argunaught::parser()
        .command(
            "version", [] () { printf("Version: 1\n"); }
        )
        .command(
            "sub", [] () { /*argunaught::opt("h", "help")*/ } 
        );
}
