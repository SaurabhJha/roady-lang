#ifndef AST_SYNTAX_TREE_H_
#define AST_SYNTAX_TREE_H_

#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "parser/parser.h"
#include "tokenizer/tokenizer.h"

namespace ast {

class SyntaxTreeNode {
 private:
  std::string data_;
  std::vector<SyntaxTreeNode> children_;

 public:
  SyntaxTreeNode() = default;
  SyntaxTreeNode(std::string data, std::vector<SyntaxTreeNode> children = {})
    :data_{std::move(data)}, children_{std::move(children)}
  {}
  ~SyntaxTreeNode() = default;
};

SyntaxTreeNode construct_syntax_tree(
    std::vector<tokenizer::Token> tokens,
    const std::vector<std::pair<
        parser::ParsingActionType,
        parser::Production>>& parser_outputs);

}  // namespace ast

#endif // AST_SYNTAX_TREE_H_
