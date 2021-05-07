#ifndef ACRAM_COMMON_H
#define ACRAM_COMMON_H

#include "lib/vector.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
namespace fs = std::filesystem;

/**
 * @file common.hpp
 * @brief Contains miscellanious small classes, non-member functions and definitions.
 */

/// Error codes that are used by @p expr_parser class and @p main function
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

/// Types of expression tree nodes
enum node_types {
    EMPTY = 0, INT, FRAC, VAR, PAR, OP
};

/// Numerical codes for operations available in expressions
enum operations {
    NONE = 0,
    ADD, SUB, MUL, DIV,
    PWR, EXP, LOG, SQRT,
    SIN, COS, TAN, COT,
    ASIN, ACOS, ATAN, ACOT
};

/// @brief Used to store value of an expression node.
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
 * Node of the expression tree.
 */
struct expr_node
{
    char type;
    expr_value value;
    expr_node* parent;
    expr_node* left;
    expr_node* right;

public:
    expr_node() = delete;

    /**
     * @brief Initialize node with value
     * @param _type node type ( @p INT, @p OP, etc.)
     * @param _value node value ( @p 1 , @p SIN, etc.)
     * @details all pointers are initialized with nullptr
     */
    expr_node(char _type, const expr_value& _value);

    /**
     * @brief Initialize node with value and connect it to subtrees
     * @param _type node type ( @p INT, @p OP, etc.)
     * @param _value node value ( @p 1 , @p SIN, etc.)
     * @param parent value to initialize @p parent field
     * @param left value to initialize @p left field
     * @param right value to initialize @p right field
     * @details Note that values of parent, left and right nodes are unchanged.
     * Caller should call @p Link function to connect nodes altogether
     * or do it manually.
     */
    expr_node(
        char _type,
        const expr_value& _value,
        expr_node* parent,
        expr_node* left,
        expr_node* right
        );

    expr_node(const expr_node& that) = delete;
    expr_node(expr_node&& that) = delete;
    expr_node& operator =(const expr_node& that) = delete;
    expr_node& operator =(expr_node&& that) = delete;
    
    /**
     * @brief Recursive destructor
     * @details Deletes left and right subtrees unless they are @p nullptr.
     * This way the caller sohuld delete only root to recursively free all memory.
     */
    ~expr_node();

    /**
     * Connect node with left and right subtrees.
     * Works correctly if any of the pointers is @p nullptr
     * @param _parent pointer to node
     * @param _left pointer to the node that becomes the left subtree
     * @param _right pointer to the node that becomes the right subtree
     */
    friend void Link(expr_node* _parent, expr_node* _left, expr_node* _right);
};

/**
 * @brief Obtain a copy of a node with both subtrees
 * @param src source object
 */
expr_node* Copy(const expr_node* src);

/**
 * @brief Generate a @p std::filesystem::path vector from command line argument vector
 * @param names_count number of strings to read from @p names
 * @param names argument array starting from the first name to convert
 */
tld::vector<fs::path> FillPathv(int names_count, char* names[]);

/**
 * @brief Extract a substring up to a delimeter
 * @param where_from string to extract from
 * @param where_to string where to place the result
 * @param pos initial position of extraction
 * @param delim delimeter character
 * @return Position in the string where the firs delimeter was met
 * If reached end of the string, npos is returned
 */
size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char delim);

/**
 * @brief Extract a substring up to a delimeter
 * @param where_from string to extract from
 * @param where_to string where to place the result
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

/** 
 * Find next non-space character in string
 * @param str string to search
 * @param pos position to search from
 * @return Position of first occurence of any non-space
 * character since @p pos
 */
inline std::size_t SkipSpaces(const std::string& str, size_t pos)
{
    while (std::isspace(str[pos]))
        pos++;
    return pos;
}

/// Find element in a vector
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
