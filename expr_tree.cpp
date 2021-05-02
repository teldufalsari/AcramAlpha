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

std::string expr_tree::toStr(const expr_node* node)
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

std::string expr_tree::texify(const expr_node& node)
{
    switch (node.type) {
    case INT:
        return std::to_string(node.value.integer);
    case FRAC:
        return std::to_string(node.value.frac);
    case OP:
        return OpToTex(node.value.integer);
    case VAR:
        return ParToTex(this->variable_);
    case PAR:
        return ParToTex(this->parameters_.at(node.value.integer));
    default:
        return "nil";
    }
}

std::string expr_tree::toTex(const expr_node* node)
{
    std::string output;
    bool need_parentheses = NeedParentheses(*node);
    if (need_parentheses)
        output += "(";
    if (node->type == OP && node->value.integer == DIV) {
        output += "{" + texify(*node) + "{" + toTex(node->left) + "}{" + toTex(node->right) + "}}";
    } else {
        output += "{";
        if (node->left != nullptr)
            output += toTex(node->left);
        output += texify(*node);
        if (node->right != nullptr)
            output += toTex(node->right);
        output += "}";
    }
    if (need_parentheses)
        output += ")";
    return output;
}

std::string expr_tree::toTex()
{
    return toTex(root_);
}

std::string OpToTex(int op)
{
    switch (op) {
    case ADD:
        return "+";
    case SUB:
        return "-";
    case MUL:
        return "\\cdot";
    case DIV:
        return "\\frac";
    case COS:
        return "cos";
    case SIN:
        return "sin";
    case TAN:
        return "tan";
    case COT:
        return "cot";
    case EXP:
        return "exp";
    case PWR:
        return "^";
    case LOG:
        return "log";
    default:
        return "nil";
    }
}

std::string ParToTex(const std::string& par)
{
    std::string output(1, par[0]);
    if (par.size() > 1) {
        output += std::string("_{") + par.substr(1) + std::string("}");
    }
    return output;
}
