#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <map>
#include <stdio.h>
#include <math.h>
#include <thread>
#include <stack>
#include <chrono>

#include <boost/numeric/ublas/matrix.hpp>

class Token {
public:
    enum class Type {
        Unknown,
        Number,
        Operator,
        Matrix,
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
    Tree(const std::string& expr, const std::map<std::string, boost::numeric::ublas::matrix<double>>& matricies);
    void BFS(Node* node);
    std::string MultithreadCompute();
    void Compute(Node* node, std::stop_source& source);
    std::deque<Token> ShuntingYard(const std::deque<Token>& tokens);
    void Fill();
    std::deque<Token> ExprToTokens(const std::string& expr);
    std::string GetErrorString();

private:
    std::vector<Node*> nodesCalc_;
    std::vector<Node> nodes_;
    Node* root_;
    const size_t max_thread_num = 4;
    std::map<std::string, boost::numeric::ublas::matrix<double>> matricies_;
    std::string errorString_;
    template <typename T, typename U>
    struct wrapper{
        std::map<std::string, std::pair<std::function<T(U, T)>, bool>> map_functions;
    };

    wrapper<double, double> wrap_;
    wrapper<boost::numeric::ublas::matrix<double>, double> matrix_to_double_;
    wrapper<boost::numeric::ublas::matrix<double>, boost::numeric::ublas::matrix<double>> matrix_to_matrix_;
};

#endif //TREE_H
