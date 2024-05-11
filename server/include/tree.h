#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <functional>
#include <map>
#include <stdio.h>
#include <math.h>
#include <thread>
#include <stack>
#include <thread>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/any.hpp>

class Token {
public:
    enum class Type {
        Unknown,
        Number,
        Operator,
        Variable,
        Function,
        LeftParen,
        RightParen,
    };

    Token(Type type, const std::string& s, int precedence, bool rightAssociative, bool unary);

    Type type_;
    std::string str_;
    const int precedence_;
    const bool rightAssociative_;
    const bool unary_;
};

struct Node{
    Node(Node* leftChild, Node* rightChild, Token value, Node* parent);
    void AddChildren(Node* leftNode, Node* rightNode);
    Node* leftChild_;
    Node* rightChild_;
    Node* parent_;
    Token value_;
};

class Tree{
public:
    Tree(const std::string& expr);
    void BFS(Node* node);
    std::string MultithreadCompute();
    void Compute(Node* node);
    std::deque<Token> ShuntingYard(const std::deque<Token>& tokens);
    void Fill();
    std::deque<Token> ExprToTokens(const std::string& expr);

private:
    std::vector<Node*> nodesCalc_;
    std::vector<Node> nodes_;
    Node* root_;
    template <typename T, typename U>
    struct wrapper{
        std::map<std::string, std::pair<std::function<T(U, T)>, bool>> map_functions;
    };

    wrapper<double, double> wrap_;
    wrapper<boost::numeric::ublas::matrix<double>, double> matrix_to_double_;
    wrapper<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::matrix<double>> matrix_to_matrix_;
};

#endif //TREE_H
