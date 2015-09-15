A toy programming language, tentatively called EL3 (embedded lisp-like language)

The main goal behind it is to create a very simple scripting language for C++ 
programs to use, with a similar syntax to lisp. It likely won't ever be a
serious alternative to a proper scripting language; this is mostly just a 
learning excercise about creating programming languages.

Will use ( ) style lists for calling functions with arguments, and { } for blocks 
which aren't evaluated immediately (and can be used to create functions). The
formal language grammar is still up in the air.

It will only have local storage on a stack. Non-scoped allocation would need to
be done C++ side and exposed via native functions.

Probable built-in functions to add:

- standard operators, +, -, *, /, %, <, >, == etc.
- 'def' or 'let' style function to add stuff to the symbol table.
- conditionals like 'if', loops with 'for', 'while' etc.

Trivial currently working example program: (1 + 2 using a lambda function)

    ({ :x -> + x 1 } 2)
