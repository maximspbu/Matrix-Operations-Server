#include "../include/tree.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

Node::Node(Node* leftChild, Node* rightChild, Token value, Node* parent = nullptr): 
    leftChild_(leftChild), rightChild_(rightChild), value_(value), parent_(parent){
    parent_ = nullptr;
}

void Node::AddChildren(Node* leftNode, Node* rightNode = nullptr){
    leftChild_ = leftNode;
    rightChild_ = rightNode;
}

Tree::Tree(const std::string& expr){
    auto queue = exprToTokens(expr);
    queue = shuntingYard(queue);
    for (Token& token: queue){
        nodes_.emplace_back(nullptr, nullptr, token);
    }
    std::stack<Node*> stack_nodes;
    for (Node& node: nodes_){
        if (node.value_.type == Token::Type::Number || node.value_.type == Token::Type::Variable){
            stack_nodes.push(&node);
        } else {
            if (node.value_.unary){
                Node* left = stack_nodes.top();
                stack_nodes.pop();
                node.AddChildren(left);
                stack_nodes.push(&node);
            } else {
                Node* right = stack_nodes.top();
                stack_nodes.pop();
                Node* left = stack_nodes.top();
                stack_nodes.pop();
                node.AddChildren(left, right);
                stack_nodes.push(&node);
            }
        }
    }
    root_ = stack_nodes.top();
}

constexpr void Tree::fill(){
    wrap.map_functions["sin"] = std::pair([](double x, auto y) { return sin(x);}, 1);
    wrap.map_functions["cos"] = std::pair([](double x, auto y) { return cos(x);}, 1);
    wrap.map_functions["max"] = std::pair([](double x, auto y) { return std::max(x, y);}, 0);
    wrap.map_functions["min"] = std::pair([](double x, auto y) { return std::min(x, y);}, 0);
    //matrix_to_matrix.map_functions["trans"] = std::pair([](boost::numeric::ublas::matrix<double> x, auto y) { return boost::numeric::ublas::trans(x);}, 1);
}

std::deque<Token> Tree::exprToTokens(const std::string& expr){
    std::deque<Token> tokens;

    for (const auto* p = expr.c_str(); *p; ++p) {
        if (isblank(*p)){
            continue;
        } else if (isdigit(*p)){
            const auto* b = p;
            while (isdigit(*p)){
                ++p;
            }
            auto s = std::string(b, p);
            tokens.push_back(Token{Token::Type::Number, s});
            --p;
        } else if (isalpha(*p)){
            const auto* b = p;
            while (isalpha(*p)){
                ++p;
            }
            auto s = std::string(b, p);
            if (*p == '('){
                int precedence = 6;
                bool rightAssociative = false;
                bool unary = true;
                tokens.push_back(Token{Token::Type::Function, s, precedence, rightAssociative, wrap.map_functions[s].second});
            } else {
                tokens.push_back(Token{Token::Type::Variable, s});
            }
            --p;
        } else {
            Token::Type t = Token::Type::Unknown;
            int precedence = -1;
            bool rightAssociative = false;
            bool unary = false;
            char c = *p;
            switch (c){
            default:                                    break;
            case '(':   t = Token::Type::LeftParen;     break;
            case ')':   t = Token::Type::RightParen;    break;
            case '^':   t = Token::Type::Operator;      precedence = 4; rightAssociative = true;  break;
            case '*':   t = Token::Type::Operator;      precedence = 3; break;
            case '/':   t = Token::Type::Operator;      precedence = 3; break;
            case '+':   t = Token::Type::Operator;      precedence = 2; break;
            case '-':
                // If current token is '-'
                // and if it is the first token, or preceded by another operator, or left-paren,
                if (tokens.empty()
                || tokens.back().type == Token::Type::Operator
                || tokens.back().type == Token::Type::LeftParen
                ) {
                    // it's unary '-'
                    // note#1 : 'm' is a special operator name for unary '-'
                    // note#2 : It has highest precedence than any of the infix operators
                    unary = true;
                    c = 'm';
                    t = Token::Type::Operator;
                    precedence = 5;
                } else {
                    // otherwise, it's binary '-'
                    t = Token::Type::Operator;
                    precedence = 2;
                }
                break;
            }
            const auto s = std::string(1, c);
            tokens.push_back(Token{t, s, precedence, rightAssociative, unary});
        }
    }

    return tokens;
}

std::deque<Token> Tree::shuntingYard(const std::deque<Token>& tokens) {
    std::deque<Token> queue;
    std::vector<Token> stack;

    // While there are tokens to be read:
    for (auto token: tokens){
        // Read a token
        switch (token.type){
        case Token::Type::Number:
            // If the token is a number, then add it to the output queue
            queue.push_back(token);
            break;
        case Token::Type::Variable:
            // If the token is a variable, then add it to the output queue
            queue.push_back(token);
            break;
        case Token::Type::Operator:
            {
                // If the token is operator, o1, then:
                const auto o1 = token;

                // while there is an operator token,
                while (!stack.empty()){
                    // o2, at the top of stack, and
                    const auto o2 = stack.back();

                    // either o1 is left-associative and its precedence is
                    // *less than or equal* to that of o2,
                    // or o1 if right associative, and has precedence
                    // *less than* that of o2,
                    if (
                        (!o1.rightAssociative && o1.precedence <= o2.precedence)
                    ||  (o1.rightAssociative && o1.precedence <  o2.precedence)){
                        // then pop o2 off the stack,
                        stack.pop_back();
                        // onto the output queue;
                        queue.push_back(o2);
                        continue;
                    }

                    // @@ otherwise, exit.
                    break;
                }

                // push o1 onto the stack.
                stack.push_back(o1);
            }
            break;

        case Token::Type::Function: // FIX
            {
                // If the token is operator, o1, then:
                //std::cout << "FLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAGFLAG\n";
                const auto o1 = token;

                // while there is an operator token,
                while (!stack.empty()){
                    // o2, at the top of stack, and
                    const auto o2 = stack.back();

                    // either o1 is left-associative and its precedence is
                    // *less than or equal* to that of o2,
                    // or o1 if right associative, and has precedence
                    // *less than* that of o2,
                    if (
                        (!o1.rightAssociative && o1.precedence <= o2.precedence)
                    ||  (o1.rightAssociative && o1.precedence < o2.precedence)){
                        // then pop o2 off the stack,
                        stack.pop_back();
                        // onto the output queue;
                        queue.push_back(o2);
                        continue;
                    }

                    // @@ otherwise, exit.
                    break;
                }

                // push o1 onto the stack.
                stack.push_back(o1);
            }
            break;

        case Token::Type::LeftParen:
            // If token is left parenthesis, then push it onto the stack
            stack.push_back(token);
            break;

        case Token::Type::RightParen:
            // If token is right parenthesis:
            {
                bool match = false;

                // Until the token at the top of the stack
                // is a left parenthesis,
                while (!stack.empty() && stack.back().type != Token::Type::LeftParen){
                    // pop operators off the stack
                    // onto the output queue.
                    queue.push_back(stack.back());
                    stack.pop_back();
                    match = true;
                }

                if (!match && stack.empty()){
                    // If the stack runs out without finding a left parenthesis,
                    // then there are mismatched parentheses.
                    printf("RightParen error (%s)\n", token.str.c_str());
                    return {};
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                stack.pop_back();
            }
            break;

        default:
            printf("error (%s)\n", token.str.c_str());
            return {};
        }
    }

    // When there are no more tokens to read:
    //   While there are still operator tokens in the stack:
    while (!stack.empty()){
        // If the operator token on the top of the stack is a parenthesis,
        // then there are mismatched parentheses.
        if (stack.back().type == Token::Type::LeftParen){
            printf("Mismatched parentheses error\n");
            return {};
        }

        // Pop the operator onto the output queue.
        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }
    return queue;
}

void Tree::BFS(Node* node){
    //std::cout << node->value_ << '\n';
    if (node->value_.type == Token::Type::Number || node->value_.type == Token::Type::Variable){
        return ;
    }
    if ((node->value_.type == Token::Type::Function || node->value_.type == Token::Type::Operator) &&
        (node->leftChild_->value_.type == Token::Type::Number || node->leftChild_->value_.type == Token::Type::Variable) &&
        (node->rightChild_ == nullptr || node->rightChild_->value_.type == Token::Type::Number || node->rightChild_->value_.type == Token::Type::Variable)){
        nodesCalc_.push_back(node);
        return ;
    }
    BFS(node->leftChild_);
    BFS(node->rightChild_);
}

void Tree::Compute(Node* node){
    switch (node->value_.type){
        case Token::Type::Operator:{
            if (node->value_.unary == 1){
                const auto lhs = stoi(node->leftChild_->value_.str);
                switch (node->value_.str[0]){
                default:
                    printf("Operator error [%s]\n", node->value_.str.c_str());
                    exit(0);
                    break;
                case 'm':  
                    node->value_.str = std::to_string(-lhs); // Special operator name for unary '-'
                    break;
                }
            } else {
                const auto lhs = stoi(node->leftChild_->value_.str);
                const auto rhs = stoi(node->rightChild_->value_.str);
                switch(node->value_.str[0]){
                default:
                    printf("Operator error [%s]\n", node->value_.str.c_str());
                    exit(0);
                    break;
                case '^':
                    node->value_.str = std::to_string(static_cast<int>(pow(lhs, rhs)));
                    break;
                case '*':
                    node->value_.str = std::to_string(lhs * rhs);
                    break;
                case '/':
                    node->value_.str = std::to_string(lhs / rhs);
                    break;
                case '+':
                    node->value_.str = std::to_string(lhs + rhs);
                    break;
                case '-':
                    node->value_.str = std::to_string(lhs - rhs);
                    break;
                }
            }
            break;
        } case Token::Type::Function: {
            if (node->value_.unary == 1){
                const auto lhs = stoi(node->leftChild_->value_.str);
                node->value_.str = std::to_string(wrap.map_functions[node->value_.str].first(static_cast<double>(lhs), 0));
            } else {
                const auto lhs = stoi(node->leftChild_->value_.str);
                const auto rhs = stoi(node->rightChild_->value_.str);
                node->value_.str = std::to_string(wrap.map_functions[node->value_.str].first(static_cast<double>(lhs), static_cast<double>(rhs)));
            }
            break;
        }
    }
    node->value_.type = Token::Type::Number;
    node->AddChildren(nullptr);
}

std::string Tree::MultithreadCompute(){
    std::vector<std::thread> threads;
    while ((root_->value_.type != Token::Type::Number) || (root_->value_.type != Token::Type::Variable)){
        BFS(root_);
        if (nodesCalc_.size() == 0){
            break;
        }
        for (Node* node: nodesCalc_){
            //std::cout << "node value: " << node->value_ << '\n';
            threads.emplace_back(&Tree::Compute, this, node);
        }
        for (std::thread& thread: threads){
            thread.join();
        }
        nodesCalc_.clear();
        threads.clear();
    }
    return root_->value_.str;
}
