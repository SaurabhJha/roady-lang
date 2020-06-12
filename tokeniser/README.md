# Tokenizer
This directory contains all the tools necessary to build a programming
language tokenizer.

* `common.h/common.cc`: Common utilities.
* `finite_automata.h/finite_automata.cc`: An implementation of non-deterministic finite automata.
* `regex.h/regex.cc`: The implementation of regular expression to finite automaton compiler.
* `tokenizer.h/tokenizer.cc`: The implementation of the lexical analyser. Conflicting tokens are decided using maximal-munch policy.