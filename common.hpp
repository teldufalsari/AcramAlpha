#ifndef ACRAM_COMMON_H
#define ACRAM_COMMON_H

#include "lib/vector.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
namespace fs = std::filesystem;

/// @file common.hpp

enum error_codes {
    OK = 0,
    ERR_NO_FILE = 1,
    ERR_CLOSING_PAR,
    ERR_NO_OPERAND,
    ERR_INVALID_OPERAND,
    ERR_NO_EXPR,
    ERR_GARBAGE,
    ERR_NO_EQUAL_SIGN
};

enum node_types {
    EMPTY = 0, INT, FRAC, VAR, PAR, OP
};

enum operations {
    NONE = 0,
    ADD, SUB, MUL, DIV,
    PWR, EXP, LOG, SQRT,
    SIN, COS, TAN, COT,
    ASIN, ACOS, ATAN, ACOT
};

union expr_value
{
public:
    long integer; // used both for integer numbers and operator codes
    double frac;

public:
    expr_value();
    expr_value(long _int);
    expr_value(double _frac);
};

/**
 * Node of expression tree.
 */
struct expr_node
{
    char type;
    expr_value value;
    expr_node* parent;
    expr_node* left;
    expr_node* right;

public:
    expr_node();
    expr_node(char _type, const expr_value& _value);
    expr_node(
        char _type,
        const expr_value& _value,
        expr_node* parent,
        expr_node* left,
        expr_node* right
        );
    ~expr_node();

    /**
     * Connect node with left and right subtrees
     * Works correctly if any of the pointers is nullptr
     */
    friend void Link(expr_node* _parent, expr_node* _left, expr_node* _right);
};

/**
 * @brief Obtain a copy of a node with both subtrees
 * @param src source object
 */
expr_node* Copy(const expr_node* src);

/// Generate a filesystem path vector from command line argument vector
tld::vector<fs::path> FillPathv(int names_count, char* names[]);

/**
 * @brief Extract a substring up to a delimeter
 * @param where_from
 * @param where_to
 * @param pos initial position of extraction
 * @param delim delimeter character
 * @return Position in the string where the firs delimeter was met
 * If reached end of the string, npos is returned
 */
size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char delim);

/**
 * @brief Extract a substring up to a delimeter
 * @param where_from
 * @param where_to
 * @param pos initial position of extraction
 * @param delim C-string that represents the delimeter set
 * @return Position in the string where the firs delimeter was met
 * If reached end of the string, npos is returned
 */
size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char* delim);

/**
 * @brief Search for the first occurence of any given character
 * @param where_from string to search
 * @param pos initial position of searching
 * @param delim C-string that represents the delimeter set
 * @return Position in the string where the firs delimeter was met
 */
size_t FindFirstOfSet(const std::string& where_from, size_t pos, const char* delim);

/// Tell whether strs string contains chrs character
bool ChrCmp(const char chrs, const char* strs);

/// Get string corresponding to operator numerical code defined in operations::
std::string OpToStr(int op);

/// Tell whether operator is commutative or not
bool IsCommutative(int op);

/// Get priority value of an operator, the less the higher
int Priority(const expr_node& node);

/// Tell if expression node needs parentheses around when it is printed
bool NeedParentheses(const expr_node& node);

/// Tell if node represents integer value of 0
bool IsZero(const expr_node* node);

/// Tell if node represents integer value of 1
bool IsOne(const expr_node* node);

/// Tell if node is a left subtree
bool IsOnLeft(const expr_node* node);

/// Tell if node represents an arithmetical operation
bool IsArith(const expr_node* node);

/// Tell if a node value can be calculated
bool IsCalculable(const expr_node* node);

/** Tell if node represents a negattive integer
 * This includes unary minus nodes with an integer argument
 */
bool IsNegative(const expr_node* node);

/// Get randomly chosen phrase
std::string Splash();

inline std::size_t SkipSpaces(const std::string& str, size_t pos)
{
    while (std::isspace(str[pos]))
        pos++;
    return pos;
}

template <typename T>
inline size_t VecFind(const tld::vector<T>& the_vector, const T& value)
{
    const T* v_data = the_vector.data();
    for (size_t i = 0; i < the_vector.size(); i++)
        if (v_data[i] == value)
            return i;
    return std::string::npos;
}

#endif // ACRAM_COMMON_H
