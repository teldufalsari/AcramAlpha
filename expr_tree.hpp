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

    std::string toStr();
    std::string toTex();

private:
    std::string toStr(const expr_node* node);
    std::string toTex(const expr_node* node);
    std::string valueToStr(const expr_node& node);
    std::string texify(const expr_node& node);
};

std::string OpToTex(int op);
std::string ParToTex(const std::string& par);

#endif // ACRAM_EXPR_TREE_H