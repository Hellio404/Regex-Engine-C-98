
# Regular Expressions Engine

This is a basic Regex Engine using recursion (YES I KNOW).
I tried to implement (using c++98) as much feature from ECMAScript regex as possible but there is a lot more to add (especially optimizations)



## Quick Reference

#### Supported features


|    Feature    |          Description       |
| :-------- | :------------------------- |
| `a` | matches the character with ASCII Code 97 ('a') |
| `abc` | matches the string 'abc' |
| `[abc]` | matches the one of the character 'a', 'b' or 'c' |
| `[^abc]` | matches any character except  'a', 'b' and 'c' |
| `[a-z]` | matches all the characters with ASCII code between 97 ('a') and 122 ('z') |
| `[^a-z]` | matches any character except  with ASCII code between 97 ('a') and 122 ('z') |
| `.`   | matches any character except (new_line) |
| `r1 \| r2` | matches the regex r1 or r2 (`a\|b` will match the character a or b)  |
|`\s`| matches a single whitespace ('`\r\n\t\f\v and space`')|
|`\S`| matches a any single character except whitespaces ('`\r\n\t\f\v and space`')|
|`\w`| the same as [a-zA-Z0-9_]|
|`\W`| the same as [^a-zA-Z0-9_]|
|`\d`| the same as [0-9]|
|`\D`| the same as [^0-9]|
|`\b`| matches, without consuming any characters, immediately between  <br />  a character matched by `\w` and a character not matched by `\w` |
|`\B`| matches, without consuming any characters, at the position between two characters matched by `\w` or `\W`|
|`^`| matches, without consuming the beginning of the line|
|`$`| matches, without consuming the beginning of end of the line|
|`a?`| matches the character 'a' zero or one time (Greedy)*|
|`(abc)?`| matches the string 'abc' zero or one time (Greedy)*|
|`a*`| the character 'a' zero or more time (Greedy)*|
|`a+`| the character 'a' one or more time (Greedy)*|
|`a{5}`| the character 'a' 5 times (Greedy)*|
|`a{5,}`| the character 'a' 5 or more times (Greedy)*|
|`a{5,8}`| the character 'a' between 5 and 8 times (Greedy)*|
|`a??`| matches the character 'a' zero or one time (Lazy)**|
|`a*?`| the character 'a' zero or more time (Lazy)**|
|`a+?`| the character 'a' one or more time (Lazy)**|
|`a{5}?`| the character 'a' 5 times (Lazy)**|
|`a{5,}?`| the character 'a' 5 or more times (Lazy)**|
|`a{5,8}?`| the character 'a' between 5 and 8 times (Lazy)**|
|`(...)`| isolate the regex inside the '()' so it can be refered to later by ID (ID start at 1) <br /> Please note that (...)+ will create only one group and it's value will get overide every repetition <br /> The (...) also used to change the priority of the operators normaly: **repetition > concatination > alternation** |
|`(?:...)`|non-capturing group (that means it doesn't get an ID and you can't refere to it later) <br /> It is used to change the priority of the operators |
|`\#`| where # is an ID of a group, it will match exactly the string (not the regex) captured by the group with ID #<br />Example: (a\|b)c\1 will match aca or bcb and not acb or bca|
|`(?=...)`| Asserts that the given subpattern can be matched here, without consuming characters |
|`(?!...)`| Asserts that the given subpattern cannot be matched here, without consuming characters |
|`(?<=...)`| Ensures that the given pattern will match, ending at the current position in the expression. <br />(the subexpression should be of fixed width (doesn't have any repetition symbol) )|
|`(?<!...)`| Ensures that the given pattern will not match, ending at the current position in the expression. <br />(the subexpression should be of fixed width (doesn't have any repetition symbol) )|


*Greedy: matches the maximum character possible. <br />
**Lazy: matches as few characters as possible.
