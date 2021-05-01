#include "expr_tree.hpp"

expr_tree::~expr_tree()
{
    delete root_;
}

expr_tree::expr_tree(expr_node* _root, const tld::vector<std::string>& _parameters, const std::string& _variable) :
    root_(_root),
    parameters_(_parameters),
    variable_(_variable)
{}

std::string expr_tree::to_str(expr_node* node)
{
    std::string output = "(";
    if (node->left != nullptr)
        output += to_str(node->left);
    output += std::to_string(node->value.integer);
    if (node->right != nullptr)
        output += to_str(node->right);
        output += ")";
    return output;
}

std::string expr_tree::to_str()
{
    return to_str(root_);
}
