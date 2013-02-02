Math expression parser written in C language
============================================

Transformer of mathematical expression like `(((((+12)*ab)*c)/d)+4)` into a tree.

It also calculates the global type of the expression following the semantic rules.
Errors are caught and displayed with details.

Written in C language.

Specs (italian)
    
    Una espressione aritmetica semplice in ANSI C è costituita da identificatori di variabili di tipo int o double,
    costanti numeriche di tipo int o double, i due operatori aritmetici unari, i cinque operatori aritmetici binari
    (due additivi e tre moltiplicativi), e le parentesi tonde.
    Scrivere un programma ANSI C che, acquisita una espressione aritmetica semplice, stampi una espressione
    aritmetica semplice ottenuta da quella acquisita inserendo delle parentesi che riflettono la precedenza e
    l'associatività degli operatori.
    Il programma deve inoltre stampare il tipo della espressione acquisita (assumere che le variabili che
    iniziano con una lettera tra a ed l siano di tipo int, le altre di tipo double).

