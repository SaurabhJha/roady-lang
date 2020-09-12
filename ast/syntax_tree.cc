#include "ast/syntax_tree.h"

#include <algorithm>

namespace ast {

SyntaxTreeNode construct_syntax_tree(
    std::vector<tokenizer::Token> tokens,
    const std::vector<std::pair<
        parser::ParsingActionType,
        parser::Production>>& parser_outputs) {
  auto token_idx = 0;
  std::deque<SyntaxTreeNode> stack;
  for (const auto& parser_output : parser_outputs) {
    auto parser_action_type = parser_output.first;
    if (parser_action_type == parser::ParsingActionType::shift) {
      auto node_data = tokens[token_idx].get_lexeme();
      auto node = SyntaxTreeNode(node_data);
      stack.push_back(node);
      token_idx += 1;
    } else if (parser_action_type == parser::ParsingActionType::reduce) {
      auto production = parser_output.second;

      std::vector<SyntaxTreeNode> syntax_definition_arguments;
      for (auto idx = 0; idx < production.get_body().size(); ++idx) {
        syntax_definition_arguments.push_back(stack.back());
        stack.pop_back();
      }
      std::reverse(syntax_definition_arguments.begin(), syntax_definition_arguments.end());

      auto syntax_definition = production.get_definition();
      if (syntax_definition.get_definition_type() == parser::SyntaxDirectedDefinitionType::copy) {
        stack.push_back(
            syntax_definition_arguments[
                syntax_definition.get_children_indices()[0]]);
      } else {
        std::vector<SyntaxTreeNode> children;
        for (auto child_idx : syntax_definition.get_children_indices()) {
          children.push_back(syntax_definition_arguments[child_idx]);
        }
        SyntaxTreeNode parent_node = SyntaxTreeNode(
            syntax_definition.get_root_data(), children);
        stack.push_back(parent_node);
      }
    } else {
      return stack.back();
    }
  }
  return stack.back();
}

}  // namespace ast

