A C++ option parser that handles sub-commands.

# Quick Example

To use argunaught, you first define global options and any subcommands, which can have their own options.

```
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
        [] (auto& parseResult) -> int 
        { 
            /*argunaught::opt("h", "help")*/ 
            return 0;
        } 
    );
```

You then parse your command line arguments with 

```
auto parseResult = args.parse(argc, argv);
```

and can use the returned `ParseResult` to see what options were found, their parameters, and if any subcommand was used.

You can check if a subcommand was found, and run it with:

```
if(parseResult.hasCommand()) {
    parseResult.runCommand();
}
```

Where the sub-command will be provided the parse result so it can look at parameters and options on its own.

You can look at option results (including their parameters) in the `ParseResult::options` vector:

```
parseResult.options[ii];
```

and you can see any positional arguments with:

```
parseResult.positionalArgs[ii]
```

## Some Notes on Usage
Options can take parameters as seen above with `-g` taking 1 parameter.  You can also define an option to take an unknown number of parameters with `-1`.  

Positional arguments start once the last option's parameters are full or with a separate `--` to mark the end of options.  E.g. `my_tool sub -a blah -- one two three` would count `one`, `two`, and `three` as positonal arguments even though the `-a` option takes an unknown number of arguments itself.


