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

std::string expr_tree::toStr(expr_node* node)
{
    std::string output;
    if (node->parent != nullptr && Priority(*(node->parent)) <= Priority(*node))
        output += "(";
    if (node->left != nullptr)
        output += toStr(node->left);
    output += valueToStr(*node);
    if (node->right != nullptr)
        output += toStr(node->right);
    if (node->parent != nullptr && Priority(*(node->parent)) <= Priority(*node))
        output += ")";
        return output;
}

std::string expr_tree::toStr()
{
    return toStr(root_);
}

std::string expr_tree::valueToStr(const expr_node& node)
{
    switch (node.type) {
    case INT:
        return std::to_string(node.value.integer);
    case FRAC:
        return std::to_string(node.value.frac);
    case OP:
        return OpToStr(node.value.integer);
    case VAR:
        return this->variable_;
    case PAR:
        return this->parameters_.at(node.value.integer);
    default:
        return "nil";
    }
}