--This is the grammar of the parser (regex search, structure and replace) function, which returns abstract syntax trees
--Patterns can be incredibly complicated, with subpattern definition and reference (including recursion)
--The function is mainly a "replace" function, but it also performs complex searches and manipulation

--This file is a valid pattern string, describing the grammar itself

--The engine interface is a single function:
--  parse(source, pattern, regex_callback, replex_callback)
--      source can be a string or a character stream
--      pattern can be a string or a character stream
--      if provided, regex_callback is a function taking a reference to a position in a string or stream and a position in the pattern, and returning a node object
--      if provided, replex_callback is a function taking a node or an array and a position in the pattern, and returning a node or array
--  returns a node object, which has the following attributes:
--      name, which is a string with the name of the capture pattern (if any)
--      content, which is a string containing the result of the find (and possibly replace) operation
--      nodes, which is an array of regular expression object


#<S>(-------------------------------------------------------Whitespace including comments
    \s*                                                   --Whitespace and
    (--<comment>([^\n]*)\n)?                              --comments are (mostly) ignored inside patterns, 
                                                          --and must be escaped to be matched literally
)*

#<regex_unit> (#S ------------------------------------------Individual unit of pattern matching for search
                                                          --
     <delimited>(\@ <open>#regex_unit <close>#regex_unit) --sequences delimited by (possibly recursively) matching <open> and <close> units
                                                          --    if captured, returns a node with the complete string, and three subnodes:
                                                          --    0 - the contents of the first match of <open>
                                                          --    1 - the contents between <open> and <close>
                                                          --    2 - the contents of the last match of <close>
                                                          --
   | <reference>(\# <name>(\w+) )                         --matches the subpattern identified by <name>
                                                          --    (which can be undefined in a declaration, but must be defined before use)
                                                          --
   | <transform>#transform                                --matches a nested regex and transforms it inline before adding it to the match object
                                                          --
   | <lookup>(\$ #replex)                                 --matches if the input stream is the same as the result of the replex pattern
                                                          --    the replex is applied to the object matched so far in the current subpattern
                                                          --
   | <special>[.%\^]                                      --more or less the usual regex specials:
                                                          --    . = any character
                                                          --    % = matches only if the previous regex_item matched a non-empty string
                                                          --    ^ = matches only if the input pattern is over (opposite to the traditional $)
                                                          --
   | <escape>(\\.)                                        --matches the escaped character, which can be one of:
                                                          --    d = [0-9]
                                                          --    D = [^0-9]
                                                          --    w = [a-zA-Z0-9_]
                                                          --    W = [^a-zA-Z0-9_]
                                                          --    s = [\ \n\t\r]
                                                          --    S = [^\ \n\t\r]
                                                          --    0 = null character
                                                          --    xHH hex character code
                                                          --    anything else matches itself
                                                          --
   | <callback>( \& [^(]+@() )                            --invokes regex_callback with a reference to the current position of the input and <callback> as parameters
                                                          --    matches if the callback returns a node object, fails otherwise
                                                          --
   | <subpattern>                                         --matches an inline subpattern, which is delimited by brackets and contains:
      ( \( (#regex_item (\| #regex_item )*)* \) )         --    a sequence of regex_items and their alternatives
                                                          --
   | <class>(\[ [^\]]* \])                                --matches a character class.
                                                          --Differently than usual regex, the contents of the [] toggle characters on and off.
                                                          --The only special characters inside the [] are:
                                                          --    ^     toggle every character code: [] matches nothing, [^] matches anything
                                                          --    a-z   toggle every character between a and z
                                                          --    \     can be used to escape itself, ^ and - 
                                                          --therefore for instance:
                                                          --    [0-93] matches all digits except for 3
                                                          --    [333^^^34] matches anything but 4, as the 3s and the ^s negate themselves
                                                          --
   | <other>.                                             --if nothing else matched before, match the literal character in the pattern
)


#<regex_repetition>(----------------------------------------Unit repetition count
                                                          --defaults to {1,1} if omitted
                                                          --
    <flag>[?+*]                                           --usual regex flags:
                                                          --    ? = optional = {0,1}
                                                          --    + = one or more = {1,-1}
                                                          --    * = any = {0,-1}
                                                          --
    | <range>( \{                                         --explicit repetition numbers
        <min>(\\d+)?                                      --minimum, defaults to 0 if omitted
        (<colon>,                                         --if present, max defaults to -1
            <max>( \\d+ | (-1) )?                         --maximum, defaults to min if present, to -1 otherwise
        )? \} )
)

#<definition>(----------------------------------------------Subpattern definition
    \#\< <name>(\w+) \>                                  --Start with a #, follwed by a mandatory name between <>
    <unit>#regex_unit                                     --then a single regex_unit (which can be a subpattern between brackets)
    <repetition>#regex_repetition?                        --and an optional repetition count
)

#<regex_item>(#S -------------------------------------------Regex item, which includes:
    <capture>(<name>/<>@\<\>%%1!?)?                       --An optional capture flag, with an optional name
    <unit>#regex_unit                                     --A mandatory regex_unit, describing what has to match
    <repetition>#regex_repetition?                        --An optional repetition count indicator
)

#<transform>(-----------------------------------------------Matches
    \/ <item>#regex_item                                  --a single regex_item
    <alternative>(\| #regex_item )*                       --(or alternatives),
    <replex>#replex                                       --and then transforms it in place
)

#<replex_unit>(#S ------------------------------------------Basic replex transform unit
                                                          --Each of the following replaces the "current object" with:
    <regex>#regex                                         --the result of applying <regex> to the contents of the current object
                                                          --
    | (<range>(<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)   --the elements of the current object (as an array) between <min> and <max>,
                                                          --with the same logic as regex_repetition,
                                                          --with the exception that if <min> or <max> are negative, they count from the end
                                                          --and that if <min> is greater than <max>, elements are returned in reverse.
                                                          --if <min>==<max> (or max is not present), returns a single object, otherwise an array
                                                          --if the current object is not an array, index 0 returns itself, 
                                                          --any other index returns an empty array
                                                          --
    | (<name>/<>@\<\>%%1!?)                               --the subelements of the current object (as an array) with name equal to <name>
                                                          --    if <name> starts with ^, selects the elements with name NOT equal to <name>
                                                          --
    | <separator>\.                                       --itself (it does nothing, it's just used as a separator to allow $1.3.5)
                                                          --
    | (\{ <interleave>([^}]*) \})                         --the contents of the current object (as an array) interleaved by the literal pattern between {}
                                                          --
    | <flatten>\%                                         --an tree flattened one level towards the top.
                                                          --    [[1,2,3], [4,5,6], [7,8,9]] becomes [1,2,3,4,5,6,7,8,9]
                                                          --    This is the only way to access subnodes of a node object, as they are exposed an array with % 
                                                          --
    | <aggregate>\@                                       --an array containing just the current object
                                                          --
    | (\( <pattern>#pattern \) )                          --if the current object is not an empty array, the result of applying <pattern> to it 
                                                          --
    | (\*\( <loop>#pattern \) )                           --an array with the results of applying <pattern> to each element of the current object (as an array)
                                                          --
    | <callback>( \& [^(]+@() )                           --the return value of replex_callback, invoked with the current object and <callback> as parameters
                                                          --
    | (\*<setname>/<>@\<\>%%1!?)                          --if the current object is an array, sets the name of each elemet to <setname>
                                                          --otherwise, set the name of the current object to <setname>
                                                          --
    | <array>(\[ <range>replex_unit*                      --an array with the result of each <range> expression,
                 <range>(\, replex_unit*)* \])            --    flattened in a single sequence
                                                          --
    | (\| <alternative>replex_unit* )                     --if the current object is an empty array, set the current object back to the source object
                                                          --and apply <alternative>. otherwise, skip <alternative>
                                                          --
    | (<root>\$)                                          --unconditionally set the current object back to the source object
                                                          --
                                                          --In any case where there is no result (e.g. a non existent index or range), returns an empty array
)

#<replex>(--------------------------------------------------Just a grouping of 
    <unit>#replex_unit*                                   --replex_units and
    #S <finalize>\!                                       --a terminator, which returns the result and allows of nesting replex into regex
)

#<pattern> (#S----------------------------------------------main transformation pattern, including:
                                                          --
    <escape>(\\.)                                         --escaped characters, which are just copied to the output
                                                          --
    | <string>@\"\"                                       --literal strings, which are just copied to the output
                                                          --
    | <definition>#definition                             --definition of subpatterns to be used while matching
                                                          --
    | <regex>#regex                                       --matching the source object through regular expressions
                                                          --
    | <replex>(\$ #replex)                                --transforming the source object, which becomes the initial "current object" for the replex
                                                          --
    | <other>.                                            --other literal characters, which are just copied to the output
)*