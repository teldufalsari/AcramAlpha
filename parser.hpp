#ifndef ACRAM_PARSER_H
#define ACRAM_PARSER_H

#include <string>
#include "common.hpp"
#include "lib/vector.h"
#include "expr_tree.hpp"

class expr_parser
{
    const std::string& str_;
    std::string variable_;
    std::string name_;
    tld::vector<std::string> parameters_;
    std::size_t parameters_count_;
    std::size_t pos_;
    int errno_;

public:
    expr_parser() = delete;
    expr_parser(const std::string& _str);
    ~expr_parser() = default;

    expr_tree read();
    int status() const;
    std::string strerror() const;

private:
    expr_node* getNumber();
    double getFrac();
    expr_node* getPower();
    expr_node* getPrimary();
    expr_node* getWord();
    expr_node* getProduct();
    expr_node* getSum();
    expr_node* getExpr();
    expr_node* getSymbol(const std::string& symbol);
    int findFunction(const std::string& func);
    void raise(int err_code);
    void getName();
};


#endif // ACRAM_PARSER_H