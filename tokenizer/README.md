# Tokenizer
This directory contains all the tools necessary to build a programming
language tokenizer_tokenizer.

* `common.h/common.cc`: Common utilities.
* `tokenizer_finite_automata.h/tokenizer_finite_automata.cc`: An implementation of non-deterministic finite automata.
* `tokenizer_regex.h/tokenizer_regex.cc`: An implementation of regular expression to finite automaton compiler.
* `tokenizer_tokenizer.h/tokenizer_tokenizer.cc`: An implementation of the lexical analyser. Conflicting tokens are decided using maximal-munch policy.