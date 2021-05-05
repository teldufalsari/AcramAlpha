
#include "parser.hpp"
#include "expr_tree.hpp"

expr_parser::expr_parser(const std::string& _str) :
    str_(_str),
    variable_(std::string()),
    name_(std::string()),
    parameters_(),
    parameters_count_(0),
    pos_(0),
    errno_(OK)
{}

int expr_parser::status() const
{
    return errno_;
}

std::string expr_parser::strerror() const
{
    switch (errno_) {
    case OK:
        return "everithing is OK";
    case ERR_CLOSING_PAR:
        return "error: expected closing parenthesis at position " + std::to_string(pos_);
    case ERR_NO_OPERAND:
        return "error: expected operand at position " + std::to_string(pos_);
    case ERR_INVALID_OPERAND:
        return "error: invalid operand at position " + std::to_string(pos_);
    case ERR_NO_EXPR:
        return "error: could not found an expression";
    case ERR_GARBAGE:
        return "error: garbage symbols found since position " + std::to_string(pos_);
    case ERR_NO_EQUAL_SIGN:
        return "ёлы-палы, мальчики и девочки, равна нету (pos = " + std::to_string(pos_) + ')';
    default:
        return "unknown error at position " + std::to_string(pos_);
    }
}

expr_tree expr_parser::read()
{
    getName();
    if (errno_)
        return expr_tree();
    pos_ = SkipSpaces(str_, pos_ + 1);
    expr_node* root = getExpr();
    Link(root, root->left, root->right);
    if (errno_) {
        delete root;
        return expr_tree();
    }
    pos_ = SkipSpaces(str_, pos_);
    if (str_[pos_] != '\0') {
        errno_ = ERR_GARBAGE;
        delete root;
        return expr_tree();
    }
    return expr_tree(root, parameters_, variable_, name_);
}

expr_node* expr_parser::getExpr()
{
    expr_node* root = nullptr;
    if(str_[pos_] == '-') {
        root = new expr_node(OP, (long)SUB);
        pos_ = SkipSpaces(str_, pos_ + 1);
        expr_node* right = getProduct();
        Link(root, nullptr, right);
        pos_ = SkipSpaces(str_, pos_);
    } else {
        if (str_[pos_] == '+')
            pos_ = SkipSpaces(str_, pos_ + 1);
        root = getProduct();
        Link(root, root->left, root->right);
        pos_ = SkipSpaces(str_, pos_);
    }
    if (errno_ != OK)
        return root;

    while (str_[pos_] == '+' || str_[pos_] == '-') {
        expr_node* new_left = root;
        root = new expr_node(OP, str_[pos_] == '+' ? (long)ADD : (long)SUB);
        root->left = new_left;
        pos_ = SkipSpaces(str_, pos_ + 1);
        root->right = getProduct();
        Link(root, root->left, root->right);
        if (errno_ != OK)
            break;
        pos_ = SkipSpaces(str_, pos_);
    }
    return root;
}

expr_node* expr_parser::getProduct()
{
    expr_node* root = getPower();
    Link(root, root->left, root->right);
    pos_ = SkipSpaces(str_, pos_);
    while (str_[pos_] == '*' || str_[pos_] == '/') {
        expr_node* new_left = root;
        root = new expr_node(OP, str_[pos_] == '*' ? (long)MUL : (long)DIV);
        root->left = new_left;
        pos_ = SkipSpaces(str_, pos_ + 1);
        root->right = getPower();
        Link(root, root->left, root->right);
    }
    return root;
}

expr_node* expr_parser::getPower()
{
    expr_node* root = getPrimary();
    Link(root, root->left, root->right);
    pos_ = SkipSpaces(str_, pos_);
    if (str_[pos_] == '^') {
        pos_ = SkipSpaces(str_, pos_ + 1);
        expr_node* new_left = root;
        root = new expr_node(OP, (long)PWR);
        root->left = new_left;
        root->right = getPrimary();
        Link(root, root->left, root->right);
    }
    return root;
}

expr_node* expr_parser::getPrimary()
{
    expr_node* root = nullptr;
    if (str_[pos_] == '(') {
        pos_ = SkipSpaces(str_, pos_ + 1);
        root = getExpr();
        Link(root, root->left, root->right);
        if (str_[pos_] != ')')
            raise(ERR_CLOSING_PAR);
        else
            pos_ = SkipSpaces(str_, pos_ + 1);
    } else if (std::isdigit(str_[pos_])) {
        root = getNumber();
        // А link??
    } else {
        root = getWord();
        Link(root, root->left, root->right);
    }
    return root;
}

expr_node* expr_parser::getNumber()
{
    expr_node* root = nullptr;
    long integer = 0;
    if (!std::isdigit(str_[pos_])) {
        raise(ERR_NO_OPERAND);
        return root;
    }
    while (std::isdigit(str_[pos_])) {
        integer *= 10;
        // Should work, but better to use a function
        // Or even read the whole number and call strtod/strtol
        integer += str_[pos_] - '0';
        pos_++;
    }
    if (str_[pos_] != '.' && str_[pos_] != ',') {
        root = new expr_node(INT, integer);
    } else {
        pos_++;
        double frac = (double)integer + getFrac();
        root = new expr_node(FRAC, frac);
    }
    pos_ = SkipSpaces(str_, pos_);
    return root;
}

double expr_parser::getFrac()
{
    double frac = 0.0;
    if (!std::isdigit(str_[pos_])) {
        raise(ERR_INVALID_OPERAND);
        return frac;
    }
    while (std::isdigit(str_[pos_])) {
        frac += str_[pos_] - '0';
        frac /= 10.0;
        pos_++;
    }
    return frac;
}

expr_node* expr_parser::getWord()
{
    expr_node* root = nullptr;
    std::string word;
    while (std::isalnum(str_[pos_]))
        word += str_[pos_++];
    if (word.size() == 0) {
        raise(ERR_NO_OPERAND);
        root = new expr_node(NONE, (long)0);
        return root;
    }
    int f_code = findFunction(word);
    if (f_code != NONE) {
        root = new expr_node(OP, (long)f_code);
        pos_ = SkipSpaces(str_, pos_);
        if (str_[pos_] == '(') {
            pos_ = SkipSpaces(str_, pos_ + 1);
            root->right = getExpr();
            Link(root, nullptr, root->right);
            if (str_[pos_] != ')')
                raise(ERR_CLOSING_PAR);
            else
                pos_ = SkipSpaces(str_, pos_ + 1);
        } else {
            raise(ERR_NO_OPERAND);
        }
    } else {
        root = getSymbol(word);
    }
    return root;
}

int expr_parser::findFunction(const std::string& func)
{
    if (func == "cos")
        return COS;
    else if (func == "sin")
        return SIN;
    else if (func == "tan" || func == "tg")
        return TAN;
    else if (func == "cot" || func == "ctg")
        return COT;
    else if (func == "exp")
        return EXP;
    else if (func == "ln" || func == "log")
        return LOG;
    else if (func == "sqrt" || func == "squirt")
        return SQRT;
    else if (func == "arcsin")
        return ASIN;
    else if (func == "arccos")
        return ACOS;
    else if (func == "arctan" || func == "arctg")
        return ATAN;
    else if (func == "arccot" || func == "arcctg")
        return ACOT;
    else
        return NONE;
}

expr_node* expr_parser::getSymbol(const std::string& symbol)
{
    expr_node* root = nullptr;
    if (symbol == variable_) {
        root = new expr_node(VAR, (long)VAR);
    } else {
        std::size_t par_num = VecFind(parameters_, symbol);
        if (par_num == std::string::npos) // If it's first occurence of this parameter
            parameters_.push_back(symbol);
        root = new expr_node(PAR, (long)VecFind(parameters_, symbol));
    }
    return root;
}

void expr_parser::raise(int err_code)
{
    if (errno_ == OK)
        errno_ = err_code;
}

void expr_parser::getName()
{
    pos_ = SkipSpaces(str_, pos_);
    pos_ = Extract(str_, name_, pos_, "( \t\n");
    if (pos_ == std::string::npos) {
        errno_ = ERR_NO_EXPR;
        return;
    }
    pos_ = SkipSpaces(str_, pos_);
    if (str_[pos_] != '(') {
        raise(ERR_NO_EXPR);
        return;
    }
    pos_ = SkipSpaces(str_, pos_ + 1);
    pos_ = Extract(str_, variable_, pos_, ") \t\n");
    if (pos_ == std::string::npos) {
        errno_ = ERR_NO_EXPR;
        return;
    }
    pos_ = SkipSpaces(str_, pos_);
    if (str_[pos_] != ')') {
        raise(ERR_CLOSING_PAR);
        return;
    }
    pos_ = SkipSpaces(str_, pos_ + 1);
    if (str_[pos_] != '=')
        raise(ERR_NO_EQUAL_SIGN);
}
