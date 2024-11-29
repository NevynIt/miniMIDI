#pragma once

/*
The main objective of this module is to load a dynamic configuration file from the SD card and update the configuration of the device.
the lua script is called at the beginning, and it in turn calls a C function that takes a string in a custom language and returns
a byte array with the data. Lua also calls a C function that takes a name and an array and sets the configuration value.

in strings in the custom language, whitespace is ignored and comments are allowed with -- lik, and strings can be split into multiple lines with \ at the end of the line.
The custom language is as follows (in extended BNF notation)
    package ::= "package" name block
    block ::=   "{" { block } "}" |
                "if" "(" condition ")" block ["else" block] |
                "while" "(" condition ")" block |
                literal |
                reference |
                label |
                flag |
                lua_expression |
                asm_expression |
                padding
    condition ::=   literal |
                    lua_expression
    padding ::= "@@" --add padding to the next word boundary
    literal ::= decimal |
                hexadecimal |
                binary |
                string |
                reference
    decimal ::= [ "-" ] digit { digit } [ "." digit { digit } ] [ "e" [ "-" ] digit { digit } ]
    hexadecimal ::= "0x" hex_digit { hex_digit } [ "." hex_digit { hex_digit } ] [ "p" [ "-" ] digit { digit } ]
    binary ::= "0b" bin_digit { bin_digit } [ "." bin_digit { bin_digit } ] [ "e" [ "-" ] digit { digit } ]
    string ::= '"' { character } '"'
    character ::= (not \ or ") | '\"' | '\\' | '\0' | '\n' | '\r' | '\t' | '\x' hex_digit hex_digit
    digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
    hex_digit ::= digit | "a" | "b" | "c" | "d" | "e" | "f" | "A" | "B" | "C" | "D" | "E" | "F"
    bin_digit ::= "0" | "1"
    label ::= "@" name ":"
    name ::= { letter | digit | "_" | "." | lua_expression }
    letter ::= "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
    lua_expression ::= "!{" any valid lua expression "}" | --the brackets are matched and strings are skipped, so there can be nested expressions
                        "${" any valid lua expression "}" | --syntactic sugar for !{return (expression)}
                        "$" name --syntactic sugar for !{retun name}
    flag ::= "!" (name | lua_expression) --these flags are interpreted by the compiler directly and set things like the format of the output (8, 16 or 32 bit int, fixed point types, float, etc...)
    asm_expression ::= "![" { asm_instruction | label } "]" --the instructions are in a custom assembly language that is converted to a byte array
    asm_instruction ::= name [ "," { asm_argument } ]
    asm_argument ::= literal | lua_expression | asm_register
    asm_register ::= "out" | "x" | "y" | "z" | "i" digit { digit } | "s" digit { digit }
    reference ::= ref_pointer | ref_values | ref_distance
    ref_pointer ::= "&" name [ "[" index "]" ]
    ref_values ::= "#" name [ "[" index "]" [ ":" index "]" ]
    ref_distance ::= "#{" name [/] "}" --the distance between the position of the first label with that name and the one with the trailing /, in bytes (24 bit value)
    index ::= { digit } | "0x" { hex_digit } | lua_expression

the compiler follows the following stages:
    1. remove comments and whitespace
    2. split the string into tokens
    3. parse the tokens into a tree
    4. compile the tree into a byte array

the lua expression is evaluated and any of its results are taken as a strings and included in the code, separated by spaces

---- the configuration objects can be packages
---- by the way, remove the zero termination in the name of the configuration objects

the compiler creates a file with the following structure:
 - table of references
    - 8 bit number of entries in the external refence table
    - for each external reference
        - 8 bit size of the name of the package
        - the package name as chars  (no zero termination)
        - 8 bit size of the name of the label inside the package - 0 represents the start of the data block
        - the label name as chars (no zero termination)
 - table of labels
    - 8 bit number of entries in the labels table
    - for each label
        - 24 bit the index of the label inside the data after linking
            - if the most significant bit is high, the label is a reference to an external reference, whose index is stored in the least significant byte
        - 8 bit size of the name of the label inside the package
        - the label name as chars
 - data
    - a series of repetitions of the following:
        - 16 bit the number of subsequent bytes that do not contain references, if 0xFFFF, the next bytes are external references, if 0x0000, the next bytes are internal references
        - either:
            - data words without references
            - a series of references (internal or external), which are
                - 8 bit for the reference number (0xFF terminates the series)
                - 16 bit for the offset in bytes after the label
                - 8 bit for how many bytes to copy. 0 means to write the 32bit pointer to the data instead

during linking, just before loading in flash, the pointer to all the references is collected
if a reference is not found, a warning is issued and a nullptr is stored instead

the linked package has the following structure:
 - 32 bit pointer to the actual data
 - table of labels
    - for each label
        - 32 bit pointer to the memory the label refers to
        - 8 bit size of the name of the label
        - the label name as chars (no zero termination)
        - padding to the next word boundary
 - data
    - the 8 bit counters are interpreted and removed
    - the references in the data are replaced with actual pointers (a 32 bit value), word alignment is user responsibility using the @@ padding directive

An example package file looks like this

----------------------------------------------
!{
    sample_width = 16
    table_size_bits = 10
    table_size = 1 << table_size_bits
    pi = 3.1416
    data_type = "int" .. "0_" .. sample_width --sighed fixed point number, just fractional bits
}

package builtin_waves
{
    !$data_type --because data_type is int0_16, each floating point result is converted to a 16 bit fixed point number

    @sin:
    !{ i = 0 }
    while ( ${ i < table_size } )
    {
        ${ sin(i * 2 * pi / table_size) }
        !{ i = i + 1 }
    }

    @sawtooth:
    !{ i = 0 }
    while ( ${ i < table_size } )
    {
        ${ sin(i * 2 * pi / table_size) }
        !{ i = i + 1 }
    }

    !uint8 --output these numbers as single bytes
    @some letters:
    65 66 67 68 

    @help:
    "Here is some useful text to help\n"
    "the samples are saved as " $data_type " in memory"

    @code:
    ![
        @getSample: --the two labels will point to the same location
        MOV x, 123
        LD y, #sin[12:13] -- take a 16 bit value (2 bytes, at index 12 and 13) after the label sin and copy them here
        MUL x, y
        CALL #fnc
        RET

        @fnc:
        MOV i1, &math.exp[10] --pointer to 10 bytes into the external reference to package math, label exp
        MOV s2, y
        LKP_$table_size i1, s2 --lookup the value at the indirect scratchpad variable 1, using the index from the scratchpad variable 2
        MUL x, out --take the output and multipy it to x
        MOV out, x
        RET

        @advance:
        MOV i1, &useful_functions[3]
        LKP i1, 2
        ADD s1, out
        RET
    ]

    @useful_functions:
    #{ fnc_count } --return how many bytes are there between here and the fnc_count/ identifier, as a 24 bit value
    &math.exp
    &wavetable.sample1
    ${ ref("sin", 23, 0) } --output a pointer to 23 bytes after the sin label
    #{ fnc_count / }

    -- Example of conditional statements and labels
    !{ feature_enabled = true }
    if ( ${ feature_enabled } )
    {
        @feature_start:
        "Feature is enabled"
    }
    else
    {
        @feature_start:
        "Feature is disabled"
    }

    -- Example of padding directive
    @@ -- pad to next word boundary

    -- Example of references and indexed access
    &lookup_table[ ${ index } ]
    #data_values[0x10:0x1F]

    -- Example of flags setting data types
    !float32 -- set output format to 32-bit floating point
    ${ math.pi } ${ math.sqrt(2) }

    -- Example of asm_expression with labels and instructions
    ![
        @loop_start:
        DEC x
        JNZ @loop_start
        @end:
    ]

    -- Example of using Lua expressions within names
    @variable_${ os.date("%H%M%S") }:
    "Timestamped variable"

    -- Example of binary and hexadecimal literals
    0b1101 0xDEADBEEF

    -- Example of multi-line string with escaped newline
    "This is a multi-line string that \
    continues on the next line." -- trailing whitespace is ignored

    -- Example of setting a flag with a Lua expression
    !{ bit_depth = 24 }
    !{ data_format = "int" .. bit_depth }
    !$data_format
}
*/
