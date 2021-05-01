#ifndef ACRAM_EXPR_TREE_H
#define ACRAM_EXPR_TREE_H

#include "common.hpp"

class expr_tree
{
    expr_node* root_;
    tld::vector<std::string> parameters_;
    std::string variable_;

public:
    expr_tree() = default;
    expr_tree(expr_node* _root, const tld::vector<std::string>& _parameters, const std::string& _variable);
    ~expr_tree();

    std::string to_str();

private:
    std::string to_str(expr_node* node);
};


#endif // ACRAM_EXPR_TREE_H