
#include "parser.hpp"
#include "expr_tree.hpp"

expr_parser::expr_parser(const std::string& _str) :
    str_(_str),
    variable_(std::string()),
    parameters_(),
    parameters_count_(0),
    pos_(0),
    errno_(OK)
{}

expr_tree expr_parser::read(int& err_code)
{
    pos_ = SkipSpaces(str_, pos_);
    pos_ = Extract(str_, variable_, pos_, " \t\n");
    if (pos_ == std::string::npos) {
        err_code = ERR_NO_EXPR;
        return expr_tree();
    }
    pos_ = SkipSpaces(str_, pos_);
    expr_node* root = getExpr();
    Link(root, root->left, root->right);
    if (errno_) {
        err_code = errno_;
        return expr_tree();
    }
    pos_ = SkipSpaces(str_, pos_);
    if (str_[pos_] != '\0') {
        err_code = ERR_GARBAGE;
        return expr_tree();
    }
    return expr_tree(root, parameters_, variable_);
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

    while (str_[pos_] == '+' || str_[pos_] == '-') {
        expr_node* new_left = root;
        root = new expr_node(OP, str_[pos_] == '+' ? (long)ADD : (long)SUB);
        root->left = new_left;
        pos_ = SkipSpaces(str_, pos_ + 1);
        root->right = getProduct();
        Link(root, root->left, root->right);
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
        pos_ = SkipSpaces(str_, pos_);
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
    int f_code = findFunction(word);
    if (f_code != NONE) {
        if (str_[pos_] == '(') {
            pos_ = SkipSpaces(str_, pos_);
            root->right = getExpr();
            Link(root, nullptr, root->right);
            if (str_[pos_] != ')')
                raise(ERR_CLOSING_PAR);
            else
                pos_ = SkipSpaces(str_, pos_);
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
    // Rewrite using map, please
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