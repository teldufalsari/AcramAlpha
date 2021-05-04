#include "expr_tree.hpp"

expr_tree::~expr_tree()
{
    delete root_;
}

expr_tree::expr_tree(expr_node* _root, const tld::vector<std::string>& _parameters, const std::string& _variable, const std::string& _name) :
    root_(_root),
    parameters_(_parameters),
    variable_(_variable),
    name_(_name),
    errno_(T_OK)
{}

std::string expr_tree::toStr(const expr_node* node)
{
    std::string output;
    if (node->parent != nullptr && Priority(*(node->parent)) <= Priority(*node))
        output += "(";
    if (node->left != nullptr)
        output += toStr(node->left);
    output += valueToStr(*node);
    if (node->right != nullptr)
        output += toStr(node->right);
    if (node->parent != nullptr && Priority(*(node->parent)) <= Priority(*node))
        output += ")";
    return output;
}

std::string expr_tree::toStr()
{
    return toStr(root_);
}

std::string expr_tree::valueToStr(const expr_node& node)
{
    switch (node.type) {
    case INT:
        return std::to_string(node.value.integer);
    case FRAC:
        return std::to_string(node.value.frac);
    case OP:
        return OpToStr(node.value.integer);
    case VAR:
        return this->variable_;
    case PAR:
        return this->parameters_.at(node.value.integer);
    default:
        return "nil";
    }
}

std::string expr_tree::texify(const expr_node& node)
{
    switch (node.type) {
    case INT:
        return std::to_string(node.value.integer);
    case FRAC:
        return std::to_string(node.value.frac);
    case OP:
        return OpToTex(node.value.integer);
    case VAR:
        return ParToTex(this->variable_);
    case PAR:
        return ParToTex(this->parameters_.at(node.value.integer));
    default:
        return "nil";
    }
}

std::string expr_tree::toTex(const expr_node* node)
{
    std::string output;
    bool need_parentheses = NeedParentheses(*node);
    if (need_parentheses)
        output += "\\left(";
    if (node->type == OP && node->value.integer == DIV) {
        output += "{" + texify(*node) + "{" + toTex(node->left) + "}{" + toTex(node->right) + "}}";
    } else  if (node->type == OP && node->value.integer == SQRT) {
        output += texify(*node) + "{" + toTex(node->right) + "}";
    } else if (node->type == OP && node->value.integer == PWR) {
        output += toTex(node->left) + texify(*node) + "{" + toTex(node->right) + "}";
    } else {
        if (node->left != nullptr)
            output += toTex(node->left);
        output += texify(*node);
        if (node->right != nullptr)
            output += toTex(node->right);
    }
    if (need_parentheses)
        output += "\\right)";
    return output;
}

std::string expr_tree::toTex()
{
    return toTex(root_);
}

std::string OpToTex(int op)
{
    switch (op) {
    case ADD:
        return "+";
    case SUB:
        return "-";
    case MUL:
        return "\\cdot ";
    case DIV:
        return "\\frac ";
    case COS:
        return "\\cos ";
    case SIN:
        return "\\sin ";
    case TAN:
        return "\\tan ";
    case COT:
        return "\\cot ";
    case EXP:
        return "\\exp ";
    case PWR:
        return "^";
    case LOG:
        return "\\log ";
    case SQRT:
        return "\\sqrt ";
    case ASIN:
        return "\\arcsin ";
    case ACOS:
        return "\\arccos ";
    case ATAN:
        return "\\arctan ";
    case ACOT:
        return "\\arccot ";
    default:
        return "nil";
    }
}

std::string ParToTex(const std::string& par)
{
    std::string output(1, par[0]);
    if (par.size() > 1) {
        output += std::string("_{") + par.substr(1) + std::string("}");
    }
    return output;
}

expr_node* expr_tree::derivative(const expr_node* node)
{
    expr_node* deriv = nullptr;
    if (node->type == OP) {
        switch (node->value.integer) {
        case ADD:
            deriv = new expr_node(OP, (long)ADD);
            deriv->left = derivative(node->left);
            deriv->right = derivative(node->right);
            Link(deriv, deriv->left, deriv->right);
            break;
        case SUB:
            deriv = new expr_node(OP, (long)SUB);
            if (node->left != nullptr)
                deriv->left = derivative(node->left);
            else
                deriv->left = nullptr;
            deriv->right = derivative(node->right);
            Link(deriv, deriv->left, deriv->right);
            break;
        case MUL:
            deriv = mulDeriv(node);
            break;
        case DIV:
            deriv = divDeriv(node);
            break;
        case SQRT:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = sqrtDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case EXP:
            deriv = expDeriv(node);
            break;
        case LOG:
            deriv = logDeriv(node);
            break;
        case PWR:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->left);
            deriv->right = pwrDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case SIN:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = sinDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case COS:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = cosDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case TAN:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = tanDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case COT:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = cotDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case ASIN:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = arcsinDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case ACOS:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = arccosDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case ATAN:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = arctanDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        case ACOT:
            deriv = new expr_node(OP, (long)MUL);
            deriv->left = derivative(node->right);
            deriv->right = arccotDeriv(node);
            Link(deriv, deriv->left, deriv->right);
            break;
        default:
            break;
        }
    } else if (node->type == VAR) {
        deriv = new expr_node(INT, (long)1, nullptr, nullptr, nullptr);
    } else {
        deriv = new expr_node(INT, (long)0, nullptr, nullptr, nullptr);
    }
    return deriv;
}

expr_tree expr_tree::derivative()
{
    return expr_tree(derivative(this->root_), this->parameters_, this->variable_, this->name_ + "'");
}

expr_node* expr_tree::mulDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)ADD);
    deriv->left = new expr_node(OP, (long)MUL, nullptr, derivative(node->left), Copy(node->right));
    Link(deriv->left, deriv->left->left, deriv->left->right);
    deriv->right = new expr_node(OP, (long)MUL, nullptr, Copy(node->left), derivative(node->right));
    Link(deriv->right, deriv->right->left, deriv->right->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::divDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)DIV);
    deriv->left = mulDeriv(node);
    deriv->left->value.integer = SUB;
    deriv->right = new expr_node(OP, (long)PWR);
    deriv->right->left = Copy(node->right);
    deriv->right->right = new expr_node(INT, (long)2);
    Link(deriv->right, deriv->right->left, deriv->right->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::sqrtDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)DIV);
    deriv->left = new expr_node(INT, (long)1, deriv, nullptr, nullptr);
    deriv->right = new expr_node(OP, (long)MUL, deriv, nullptr, nullptr);
    expr_node* denum = deriv->right;
    denum->left = new expr_node(INT, (long)2);
    denum->right = Copy(node);
    Link(denum, denum->left, denum->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::expDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)MUL);
    deriv->left = Copy(node);
    deriv->right = derivative(node->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::logDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)DIV, nullptr, derivative(node->right), Copy(node->right));
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::pwrDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)MUL);
    deriv->left = Copy(node->right);
    deriv->right = new expr_node(OP, (long)PWR);
    auto new_pwr = new expr_node(OP, (long)SUB);
    new_pwr->left = Copy(node->right);
    new_pwr->right = new expr_node(INT, (long)1);
    Link(new_pwr, new_pwr->left, new_pwr->right);
    deriv->right->right = new_pwr;
    deriv->right->left = Copy(node->left);
    Link(deriv->right, deriv->right->left, deriv->right->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::sinDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)COS);
    deriv->right = Copy(node->right);
    Link(deriv, nullptr, deriv->right);
    return deriv;
}

expr_node* expr_tree::cosDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)SUB);
    deriv->right = new expr_node(OP, (long)SIN);
    deriv->right->right = Copy(node->right);
    Link(deriv->right, nullptr, deriv->right->right);
    Link(deriv, nullptr, deriv->right);
    return deriv;
}

expr_node* expr_tree::tanDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)DIV);
    deriv->left = new expr_node(INT, (long)1);
    deriv->right = new expr_node(OP, (long)PWR);
    auto square = deriv->right;
    square->left = new expr_node(OP, (long)COS);
    square->left->right = Copy(node->right);
    Link(square->left, nullptr, square->left->right);
    square->right = new expr_node(INT, (long)2);
    Link(square, square->left, square->right);
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::cotDeriv(const expr_node* node)
{
    auto frac = tanDeriv(node);
    frac->right->left->value.integer = SIN;
    auto deriv = new expr_node(OP, (long)SUB);
    deriv->right = frac;
    Link(deriv, nullptr, deriv->right);
    return deriv;
}

expr_node* expr_tree::arcsinDeriv(const expr_node* node)
{
    auto square = new expr_node(OP, (long)PWR, nullptr, Copy(node->right), new expr_node(INT, (long)2));
    Link(square, square->left, square->right);
    auto sub = new expr_node(OP, (long)SUB, nullptr, new expr_node(INT, (long)1), square);
    Link(sub, sub->left, sub->right);
    auto root = new expr_node(OP, (long)SQRT, nullptr, nullptr, sub);
    Link(root, root->left, root->right);
    auto frac = new expr_node(OP, (long)DIV, nullptr, new expr_node(INT, (long)1), root);
    Link(frac, frac->left, frac->right);
    return frac;
}

expr_node* expr_tree::arccosDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)SUB, nullptr, nullptr, arcsinDeriv(node));
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

expr_node* expr_tree::arctanDeriv(const expr_node* node)
{
    auto square = new expr_node(OP, (long)PWR, nullptr, Copy(node->right), new expr_node(INT, (long)2));
    Link(square, square->left, square->right);
    auto sum = new expr_node(OP, (long)ADD, nullptr, new expr_node(INT, (long)1), square);
    Link(sum, sum->left, sum->right);
    auto frac = new expr_node(OP, (long)DIV, nullptr, new expr_node(INT, (long)1), sum);
    Link(frac, frac->left, frac->right);
    return frac;
}

expr_node* expr_tree::arccotDeriv(const expr_node* node)
{
    auto deriv = new expr_node(OP, (long)SUB, nullptr, nullptr, arctanDeriv(node));
    Link(deriv, deriv->left, deriv->right);
    return deriv;
}

void expr_tree::simplify(expr_node* node)
{
    if (node->type != OP)
        return;
    if (node->left)
        simplify(node->left);
    if (node->right)
        simplify(node->right);
    if (IsCalculable(node)) {
        calcSimplifs(node);
        return;
    }
    switch (node->value.integer) {
    case ADD:
        addSimplifs(node);
        break;
    case SUB:
        subSimplifs(node);
        break;
    case MUL:
        mulSimplifs(node);
        break;
    case DIV:
        divSimplifs(node);
        break;
    case PWR:
        pwrSimplifs(node);
        break;
    default:
        break;
    }
}

void expr_tree::simplify()
{
    simplify(root_);
}

const std::string& expr_tree::getName()
{
    return name_;
}

std::string expr_tree::getVar()
{
    return ParToTex(this->variable_);
}

int expr_tree::checkSemantics(const expr_node* node)
{
    if (node == nullptr)
        return T_OK;
    if (node->type != OP)
        return T_OK;
    int state = checkSemantics(node->left);
    if (state != T_OK)
        return state;
    state = checkSemantics(node->right);
    if (state != T_OK)
        return state;
    return checkNode(node);
}

int expr_tree::checkNode(const expr_node* node)
{
    if (node->value.integer == PWR &&
        IsZero(node->left) &&
        IsZero(node->right))
        return T_ZERO_PWR;

    if (node->value.integer == DIV && IsZero(node->right))
        return T_ZERO_DIVIZION;

    if (node->value.integer == LOG && IsZero(node->right))
        return T_LOG_ZERO;

    if (node->value.integer == LOG && IsNegative(node->right))
        return T_NEGATIVE_ARG;

    return T_OK;
}

void expr_tree::checkSemantics()
{
    errno_ = checkSemantics(root_);
}

std::string expr_tree::strerror()
{
    switch (errno_)
    {
    case T_ZERO_PWR:
        return "error: zero power of zero term detected";
    case T_ZERO_DIVIZION:
        return "error: division by zero detected";
    case T_LOG_ZERO:
        return "error: logarithm of zero";
    case T_NEGATIVE_ARG:
        return "error: logarithm of a negative number";
    default:
        return "semantic error";
        break;
    }
}

int expr_tree::status()
{
    return errno_;
}

void expr_tree::mulSimplifs(expr_node* node)
{
    
    if (IsZero(node->left) || IsZero(node->right)) {
        delete node->left;
        delete node->right;
        node->left = node->right = nullptr;
        node->type = INT;
        node->value.integer = 0;
    } else if (IsOne(node->left)) {
        expr_node* tmp = node->right;
        delete node->left;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    } else if (IsOne(node->right)) {
        expr_node* tmp = node->left;
        delete node->right;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    }
}

void expr_tree::divSimplifs(expr_node* node)
{
    if (IsZero(node->left)) {
        delete node->left;
        delete node->right;
        node->left = node->right = nullptr;
        node->type = INT;
        node->value.integer = 0;
    } else if (IsOne(node->right)) {
        expr_node* tmp = node->left;
        delete node->right;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    } else if (IsOne(node->left) && node->parent != nullptr && node->parent->type == OP && node->parent->value.integer == MUL) {
        if (IsOnLeft(node)) {
            node->parent->value.integer = DIV;
            Link(node->parent, node->parent->right, node->right);
            node->right = nullptr;
            delete node;
        } else {
            node->parent->value.integer = DIV;
            Link(node->parent, node->parent->left, node->right);
            node->right = nullptr;
            delete node;
        }
    }
}

void expr_tree::addSimplifs(expr_node* node)
{
    if (IsZero(node->left)) {
        expr_node* tmp = node->right;
        delete node->left;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    } else if (IsZero(node->right)) {
        expr_node* tmp = node->left;
        delete node->right;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    }
}

void expr_tree::subSimplifs(expr_node* node)
{
    if (IsZero(node->left)) {
        delete node->left;
        node->left = nullptr;
    } else if (IsZero(node->right)) {
        if (node->left == nullptr) {// Unary minus
            delete node->right;
            node->right = node->left = nullptr;
            node->type = INT;
            node->value.integer = 0;
        } else {
            expr_node* tmp = node->left;
            delete node->right;
            node->type = tmp->type;
            node->value = tmp->value;
            Link(node, tmp->left, tmp->right);
            tmp->left = tmp->right = nullptr;
            delete tmp;
        }
    }
}

void expr_tree::calcSimplifs(expr_node* node)
{
    if (node->value.integer == DIV && (node->left->value.integer % node->right->value.integer))
        return;
    switch (node->value.integer) {
    case ADD:
        node->value.integer = node->left->value.integer + node->right->value.integer;
        break;
    case SUB:
        node->value.integer = node->left->value.integer - node->right->value.integer;
        break;
    case MUL:
        node->value.integer = node->left->value.integer * node->right->value.integer;
        break;
    case DIV:
        node->value.integer = node->left->value.integer / node->right->value.integer;
        break;
    default:
        break;
    }
    node->type = INT;
    delete node->left;
    delete node->right;
    node->left = node->right = nullptr;
}

void expr_tree::pwrSimplifs(expr_node* node)
{
    if (IsZero(node->right)) {
        delete node->left;
        delete node->right;
        node->left = node->right = nullptr;
        node->type = INT;
        node->value.integer = 1;
    } else if (IsOne(node->right)) {
        expr_node* tmp = node->left;
        delete node->right;
        node->type = tmp->type;
        node->value = tmp->value;
        Link(node, tmp->left, tmp->right);
        tmp->left = tmp->right = nullptr;
        delete tmp;
    }
}
