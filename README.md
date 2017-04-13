Math expression parser and type checker - C language
====================================================

Parser and type checker of mathematical expression like `(((((+12)*ab)*c)/d)+4)` composed by integers, floats, constants, arithmetic operators (2 unary and 5 binary), parentheses (infinite nesting levels). 

Other features:
 * The given expression is validate and also rewritten inserting parentheses reflecting the operators' priority.
 * Calculation of global type of the expression (e.g. considering C-language casting). Variables A-L* are int, M-Z are floats.
 * Error validation
 * Optimized, minimum amount of memory used
