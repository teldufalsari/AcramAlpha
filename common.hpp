#ifndef ACRAM_COMMON_H
#define ACRAM_COMMON_H

#include "lib/vector.h"
#include "lib/map.h"
#include <filesystem>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;

enum error_codes {
    OK = 0,
    ERR_NO_FILE = 1,
    ERR_CLOSING_PAR,
    ERR_NO_OPERAND,
    ERR_INVALID_OPERAND,
    ERR_NO_EXPR,
    ERR_GARBAGE
};

enum node_types {
    EMPTY = 0, INT, FRAC, VAR, PAR, OP
};

enum operations {
    NONE = 0,
    ADD, SUB, MUL, DIV,
    PWR, EXP, LOG,
    SIN, COS, TAN, COT
};

union expr_value
{
public:
    long integer;
    double frac;

public:
    expr_value();
    expr_value(long _int);
    expr_value(double _frac);
};

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

    friend void Link(expr_node* _parent, expr_node* _left, expr_node* _right);
};

tld::vector<fs::path> FillPathv(int names_count, char* names[]);
size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char delim);
size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char* delim);
size_t FindFirstOfSet(const std::string& where_from, size_t pos, const char* delim);
bool ChrCmp(const char chrs, const char* strs);
std::string OpToStr(int op);
bool IsCommutative(int op);
int Priority(const expr_node& node);

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
