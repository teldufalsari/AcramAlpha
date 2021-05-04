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

expr_node* Copy(const expr_node* src)
{
    if (src == nullptr)
        return nullptr;
    expr_node* dst = new expr_node(src->type, src->value, nullptr, nullptr, nullptr);
    if (src->left) {
        dst->left = Copy(src->left);
        dst->left->parent = dst;
    }
    if (src->right) {
        dst->right = Copy(src->right);
        dst->right->parent = dst;
    }
    return dst;
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

std::string OpToStr(int op)
{
    switch (op) {
    case ADD:
        return "+";
    case SUB:
        return "-";
    case MUL:
        return "*";
    case DIV:
        return "/";
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

bool IsCommutative(int op)
{
    return ((op == DIV) || (op == SUB) || (op == PWR) ? false : true);
}

int Priority(const expr_node& node)
{
    if (node.type == OP) {
    switch (node.value.integer) {
        case COS:
        case SIN:
        case TAN:
        case COT:
        case EXP:
        case LOG:
            return 3;
        
        case PWR:
            return 2;
        
        case MUL:
        case DIV:
            return 4;
        
        case ADD:
        case SUB:
            return 5;
        
        default:
            return -1;
        }
    } else {
        return 1;
    }
}

bool NeedParentheses(const expr_node& node)
{
    if (node.parent == nullptr) {
        return false;
    } else if (node.parent->value.integer == DIV || node.parent->value.integer == SQRT) {
        return false;
    } else if (!IsOnLeft(&node) && node.parent->value.integer == PWR) {
        return false;
    }   else if (node.value.integer == PWR && node.parent->value.integer == PWR && IsOnLeft(&node)) {
        return true;
    } else if (Priority(*(node.parent)) < Priority(node)) {
        return true;
    } else if (
        Priority(*(node.parent)) == Priority(node)
        && node.parent->right == &node
        && !IsCommutative(node.parent->value.integer)
        ) {
        return true;
    } else {
        return false;
    }
}

bool IsZero(const expr_node* node)
{
    if (node == nullptr)
        return false;
    
    if (node->type == INT && node->value.integer == 0)
        return true;
    else
        return false;
}

bool IsOne(const expr_node* node)
{
    if (node == nullptr)
        return false;
    if (node->type == INT && node->value.integer == 1)
        return true;
    else
        return false;
}

bool IsOnLeft(const expr_node* node)
{
    if (node->parent == nullptr)
        return false;
    else if (node->parent->left == node)
        return true;
    else
        return false;
}

bool IsArith(const expr_node* node)
{
    if (
        node->type == OP &&
        (node->value.integer == ADD || node->value.integer == SUB ||
        node->value.integer == MUL || node->value.integer == DIV)
    )
        return true;
    else
        return false;
}

bool IsCalculable(const expr_node* node)
{
    if (node->left == nullptr || node->right == nullptr)
        return false;
    if (
        IsArith(node) &&
        node->left->type == INT &&
        node->right->type == INT
    )
        return true;
    else
        return false;
}

static const char* splashes[] = {
    "Утрём нос Стивену Вольфраму!\n",
    "Наши логарифмы 100\\% натуральные!\n",
    "Путём несложных, но громоздких преобразованиий, очевидно, получаем:\n",
    "Ну тут всё и так очевидно\n",
    "Производная? Да, производная\\dots\n"
    };

std::string Splash()
{
    std::default_random_engine randomizer;
    unsigned seed = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();
    randomizer.seed(seed);
    unsigned index = randomizer() % (sizeof(splashes) / sizeof(const char*));
    return std::string(splashes[index]);
}
