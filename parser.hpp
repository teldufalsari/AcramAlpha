#ifndef ACRAM_PARSER_H
#define ACRAM_PARSER_H

#include <string>
#include "common.hpp"
#include "lib/vector.h"
#include "expr_tree.hpp"
/**
 * @file parser.hpp
 * @brief expression parser class
 */

/// Recursive descend parser for function definitions
class expr_parser
{
    // String overlooked by parser
    const std::string& str_;

    // Name of the main variable of the function
    std::string variable_;

    // Name of the function
    std::string name_;

    // List of symbolic constant parameters that were met in the function
    tld::vector<std::string> parameters_;
    std::size_t parameters_count_;

    // Used as a caret while parsing
    std::size_t pos_;

    int errno_;

public:
    expr_parser() = delete;
    /** Create a parser bound to a string
     * @param _str
     */
    expr_parser(const std::string& _str);
    ~expr_parser() = default;

    /// Read and parse bound string
    expr_tree read();

    /// Obtain parsing status
    int status() const;

    /// Get string describing a parsing error (if status is non-zero)
    std::string strerror() const;

private:
    // These are the methods used to read tokens from input
    expr_node* getNumber();
    double getFrac();
    expr_node* getPower();
    expr_node* getPrimary();
    expr_node* getWord();
    expr_node* getProduct();
    expr_node* getSum();
    expr_node* getExpr();
    expr_node* getSymbol(const std::string& symbol);

    // Read function name and variable
    void getName();

    // Searches for function and returns its numerical code. If not found, NONE returned
    int findFunction(const std::string& func);

    // Setter for errno_
    void raise(int err_code);
};

#endif // ACRAM_PARSER_H
