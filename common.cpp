#include "common.hpp"

expr_value::expr_value() :
    integer(0)
{}

expr_value::expr_value(long _int) :
    integer(_int)
{}
expr_value::expr_value(double _frac) :
    frac(_frac)
{}

expr_node::expr_node(
    char _type,
    const expr_value& _value
    ) :
    type(_type),
    value(_value),
    parent(nullptr),
    left(nullptr),
    right(nullptr)
{}

expr_node::expr_node(
    char _type,
    const expr_value& _value,
    expr_node* _parent,
    expr_node* _left,
    expr_node* _right
    ) :
    type(_type),
    value(_value),
    parent(_parent),
    left(_left),
    right(_right)
{}

expr_node::~expr_node()
{
    if (left != nullptr)
        delete left;
    if (right != nullptr)
        delete right;
}

tld::vector<fs::path> FillPathv(int names_count, char* names[])
{
    tld::vector<fs::path> pathv;
    for (int i = 0; i < names_count; i++) {
        fs::path current_path(names[i]);
        if (!fs::exists(current_path)) {
            std::cout << "File " << current_path <<
                " does not exist and would not be differentiated" << std::endl;
            continue;
        }
        if (!fs::is_regular_file(current_path)) {
            std::cout << "File " << current_path <<
            " is not a regular file, I won't touch it" << std::endl;;
            continue;
        }
        pathv.push_back(current_path);
    }
    return pathv;
}

void Link(expr_node* _parent, expr_node* _left, expr_node* _right)
{
    if (_parent != nullptr) {
        _parent->left = _left;
        _parent->right = _right;
    }
    if (_left != nullptr)
        _left->parent = _parent;
    if (_right != nullptr)
        _right->parent = _parent;
}

size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char delim)
{
    size_t end = where_from.find_first_of(delim, pos);
    
    if (end != std::string::npos)
        where_to = where_from.substr(pos, end - pos);
    else
        where_to = where_from;
    
    return end;
}

size_t Extract(const std::string& where_from, std::string& where_to, size_t pos, const char* delim)
{
    size_t end = FindFirstOfSet(where_from, pos, delim);

    if (end != std::string::npos)
        where_to = where_from.substr(pos, end - pos);
    else
        where_to = where_from;

    return end;
}

size_t FindFirstOfSet(const std::string& where_from, size_t pos, const char* delim)
{ 
    while ((!ChrCmp(where_from[pos], delim)) && (pos < where_from.size()))
        pos++;
    
    if (pos == where_from.size())
        return std::string::npos;
    else
        return pos;
}

bool ChrCmp(const char chrs, const char* strs)
{
    for (unsigned i = 0; strs[i] != '\0'; i++)
    {
        if (chrs == strs[i])
            return true;
    }
    return false;
}