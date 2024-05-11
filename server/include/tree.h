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

    Token(Type type,
          const std::string& s,
          int precedence = -1,
          bool rightAssociative = false,
          bool unary = false
    )
        : type { type }
        , str ( s )
        , precedence { precedence }
        , rightAssociative { rightAssociative }
        , unary { unary }
    {}

    Type type;
    std::string str;
    const int precedence;
    const bool rightAssociative;
    const bool unary;
};

struct Node{
    Node();
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
    std::deque<Token> shuntingYard(const std::deque<Token>& tokens);
    constexpr void fill();
    std::deque<Token> exprToTokens(const std::string& expr);

private:
    std::vector<Node*> nodesCalc_;
    std::vector<Node> nodes_;
    Node* root_;
    template <typename T, typename U>
    struct wrapper{
        std::map<std::string, std::pair<std::function<T(U, T)>, bool>> map_functions;
    };

    wrapper<double, double> wrap;
    wrapper<boost::numeric::ublas::matrix<double>, double> matrix_to_double;
    wrapper<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::matrix<double>> matrix_to_matrix;
};

#endif //TREE_H
