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
    expr_tree derivative();
    void simplify();

private:
    std::string toStr(const expr_node* node);
    std::string toTex(const expr_node* node);
    std::string valueToStr(const expr_node& node);
    std::string texify(const expr_node& node);
    expr_node* derivative(const expr_node* node);
    expr_node* mulDeriv(const expr_node* node);
    expr_node* divDeriv(const expr_node* node);
    expr_node* sqrtDeriv(const expr_node* node);
    expr_node* expDeriv(const expr_node* node);
    expr_node* pwrDeriv(const expr_node* node);
    expr_node* sinDeriv(const expr_node* node);
    expr_node* cosDeriv(const expr_node* node);
    expr_node* tanDeriv(const expr_node* node);
    expr_node* cotDeriv(const expr_node* node);

    void simplify(expr_node* node);
    void mulSimplifs(expr_node* node);
    void divSimplifs(expr_node* node);
    void addSimplifs(expr_node* node);
    void subSimplifs(expr_node* node);

};

std::string OpToTex(int op);
std::string ParToTex(const std::string& par);

#endif // ACRAM_EXPR_TREE_H