#ifndef ACRAM_EXPR_TREE_H
#define ACRAM_EXPR_TREE_H

#include "common.hpp"
/**
 * @file expr_tree.hpp
 * @brief expression tree class
 */

enum semantic_errors {
    T_OK,
    T_ZERO_PWR,
    T_ZERO_DIVIZION,
    T_NEGATIVE_ARG,
    T_LOG_ZERO
};

/// Expression tree class that can simplify itself and calculate its derivative
class expr_tree
{
    expr_node* root_;
    // Inherited from parser or antiderivative
    tld::vector<std::string> parameters_;
    // Inherited from parser or antiderivative
    std::string variable_;
    // Inherited from parser or antiderivative
    std::string name_;

    // For semantic check
    int errno_;

public:
    /**
     * @brief Default constructor
     * @details Used when an empty tree is required as a null value
     */
    expr_tree() = default;

    /**
     * @brief Construct normal expression tree
     * @details This is the constructor that is normally used by other functions
     */
    expr_tree(expr_node* _root, const tld::vector<std::string>& _parameters, const std::string& _variable, const std::string& _name);
    
    expr_tree(const expr_tree& that) = delete;
    expr_tree(const expr_tree&& that) = delete;
    expr_tree& operator =(const expr_tree& that) = delete;
    expr_tree& operator =(expr_tree&& that) = delete;

    /// Recursively frees allocated memory
    ~expr_tree();

    /// Get representation of the expression in LaTeX commands
    std::string toTex();

    /// Get derivative of the expression
    expr_tree derivative();

    /**
     * Simplify the expression
     * This method modifies the object
     */
    void simplify();

    /// @return Name of the function (for 'f(x)' it would be 'f')
    const std::string& getName();

    /// @return Main variable of the function (for 'f(x)' it would be 'x')
    std::string getVar();

    /**
     * Check if the expression is semantically correct
     * (Or, to be more precise, if it's not explicitly incorrect)
     */
    void checkSemantics();

    /// Get string describing semantic error
    std::string strerror();

    /// Get status of semantic check
    int status();

private:
    
    // Get LaTeX representation of a node
    std::string toTex(const expr_node* node);

    // Get LaTeX representation of node's value
    // toTex method traverses the tree applying this method to nodes
    std::string texify(const expr_node& node);

    // Recursively calculates derivative of node
    expr_node* derivative(const expr_node* node);

    // The following methods define rules of differentiation //

    expr_node* mulDeriv(const expr_node* node);
    expr_node* divDeriv(const expr_node* node);
    expr_node* sqrtDeriv(const expr_node* node);
    expr_node* expDeriv(const expr_node* node);
    expr_node* logDeriv(const expr_node* node);
    expr_node* pwrDeriv(const expr_node* node);
    expr_node* sinDeriv(const expr_node* node);
    expr_node* cosDeriv(const expr_node* node);
    expr_node* tanDeriv(const expr_node* node);
    expr_node* cotDeriv(const expr_node* node);
    expr_node* arcsinDeriv(const expr_node* node);
    expr_node* arccosDeriv(const expr_node* node);
    expr_node* arctanDeriv(const expr_node* node);
    expr_node* arccotDeriv(const expr_node* node);

    // Recursively simplify expression
    void simplify(expr_node* node);

    // The following methods define rules of simplification //
    
    void mulSimplifs(expr_node* node);
    void divSimplifs(expr_node* node);
    void addSimplifs(expr_node* node);
    void subSimplifs(expr_node* node);
    void calcSimplifs(expr_node* node);
    void pwrSimplifs(expr_node* node);

    // Recursively search for explicit semantic error
    int checkSemantics(const expr_node* node);

    // Non-recursive checking method.
    // checkSemantics method traverses the tree applying this methos to nodes
    int checkNode(const expr_node* node);
};

/// Get LaTex command corresponding to operator code
std::string OpToTex(int op);

/**
 * @brief Get LaTeX representation of a parameter or a variable name
 * @details All characters but the first are rendered as lower index
 */
std::string ParToTex(const std::string& par);

#endif // ACRAM_EXPR_TREE_H