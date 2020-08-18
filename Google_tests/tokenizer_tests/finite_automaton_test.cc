#include "gtest/gtest.h"

#include "tokenizer/finite_automaton.h"

class FiniteAutomatonTest : public ::testing::Test {
 protected:
  tokenizer::NonDeterministicFiniteAutomaton simple_automaton1;
  tokenizer::NonDeterministicFiniteAutomaton simple_automaton2;
  tokenizer::NonDeterministicFiniteAutomaton simple_automaton3;
  tokenizer::DeterministicFiniteAutomaton automaton;

  void SetUp() override {
    simple_automaton1 = tokenizer::NonDeterministicFiniteAutomaton("a");
    simple_automaton2 = tokenizer::NonDeterministicFiniteAutomaton("b");
    simple_automaton3 = tokenizer::NonDeterministicFiniteAutomaton("c");
    simple_automaton1.merge_on_union(simple_automaton2);
    simple_automaton1.apply_star();
    simple_automaton3.merge_on_concatenation(simple_automaton1);
    automaton = simple_automaton3.convert_to_dfa();  }
};

TEST_F(FiniteAutomatonTest, AutomatonAccepts) {
  automaton.move("c");
  automaton.move("b");
  automaton.move("a");
  automaton.move("b");
  automaton.move("a");

  EXPECT_EQ(automaton.has_accepted(), true);
  EXPECT_EQ(automaton.is_dead(), false);
}

TEST_F(FiniteAutomatonTest, AutomatonDies) {
  automaton.move("c");
  automaton.move("b");
  automaton.move("a");
  automaton.move("b");
  automaton.move("c");

  EXPECT_EQ(automaton.has_accepted(), false);
  EXPECT_EQ(automaton.is_dead(), true);
}

TEST_F(FiniteAutomatonTest, AutomatonReset) {
  automaton.move("c");
  automaton.move("b");
  automaton.move("a");
  automaton.move("b");
  automaton.move("c");
  automaton.reset();

  EXPECT_EQ(automaton.has_accepted(), false);
  EXPECT_EQ(automaton.is_dead(), false);
}
