#include "../include/tree.h"


Token::Token(Type type, const std::string& s, int precedence = -1, bool rightAssociative = false, bool unary = false)
    : type_ { type } , str_ ( s ), precedence_ { precedence }, rightAssociative_ { rightAssociative }, unary_ { unary }
{
    
}

Node::Node(Node* leftChild, Node* rightChild, Token value, Node* parent = nullptr)
    : leftChild_(leftChild), rightChild_(rightChild), value_(value), parent_(parent)
{
    parent_ = nullptr;
}

void Node::AddChildren(Node* leftNode, Node* rightNode = nullptr)
{
    leftChild_ = leftNode;
    rightChild_ = rightNode;
}

Tree::Tree(const std::string& expr, const std::map<std::string, boost::numeric::ublas::matrix<double>>& matricies)
    : errorString_("")
{
    matricies_ = std::move(matricies);
    Fill();
    auto queue = ExprToTokens(expr);
    queue = ShuntingYard(queue);
    if (!errorString_.empty()){
        return ;
    }
    for (Token& token: queue){
        nodes_.emplace_back(nullptr, nullptr, token);
    }
    std::stack<Node*> stack_nodes;
    for (Node& node: nodes_){
        if (node.value_.type_ == Token::Type::Number || node.value_.type_ == Token::Type::Matrix){
            stack_nodes.push(&node);
        } else {
            if (node.value_.unary_){
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

void Tree::Fill(){
    wrap_.map_functions["sin"] = std::pair([](double x, auto y) { return sin(x);}, 1);
    wrap_.map_functions["cos"] = std::pair([](double x, auto y) { return cos(x);}, 1);
    wrap_.map_functions["max"] = std::pair([](double x, auto y) { return std::max(x, y);}, 0);
    wrap_.map_functions["min"] = std::pair([](double x, auto y) { return std::min(x, y);}, 0);
    //matrix_to_matrix_.map_functions["trans"] = std::pair([](boost::numeric::ublas::matrix<double> x, auto y) { return boost::numeric::ublas::trans(x);}, 1);
}

std::deque<Token> Tree::ExprToTokens(const std::string& expr)
{
    std::deque<Token> tokens;

    for (const auto* p = expr.c_str(); *p; ++p) {
        if (isblank(*p)){
            continue;
        } else if (isdigit(*p)){
            bool point = 0;
            const auto* b = p;
            while (isdigit(*p) || (*p == '.')){
                ++p;
                if (*p == '.' && point == 0){
                    point = 1;
                    continue;
                } else if (*p == '.' && point == 1) {
                    errorString_ = "Incorrect points count!\n";
                    std::cerr << "Incorrect points count!\n";
                    return {};
                }
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
                tokens.push_back(Token{Token::Type::Function, s, precedence, rightAssociative, wrap_.map_functions[s].second});
            } else {
                tokens.push_back(Token{Token::Type::Matrix, s});
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
                || tokens.back().type_ == Token::Type::Operator
                || tokens.back().type_ == Token::Type::LeftParen
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

std::deque<Token> Tree::ShuntingYard(const std::deque<Token>& tokens) {
    std::deque<Token> queue;
    std::vector<Token> stack;
    // While there are tokens to be read:
    for (const auto& token: tokens){
        // Read a token
        switch (token.type_){
        case Token::Type::Number:
            // If the token is a number, then add it to the output queue
            queue.push_back(token);
            break;
        case Token::Type::Matrix:
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
                        (!o1.rightAssociative_ && o1.precedence_ <= o2.precedence_)
                    ||  (o1.rightAssociative_ && o1.precedence_ <  o2.precedence_)){
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
                        (!o1.rightAssociative_ && o1.precedence_ <= o2.precedence_)
                    ||  (o1.rightAssociative_ && o1.precedence_ < o2.precedence_)){
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
                while (!stack.empty()){
                    // pop operators off the stack
                    // onto the output queue.
                    if (stack.back().type_ == Token::Type::LeftParen){
                        match = true; // GOVNO
                        break;
                    }
                    queue.push_back(stack.back());
                    stack.pop_back();
                }
                if (!match && stack.empty()){
                    // If the stack runs out without finding a left parenthesis,
                    // then there are mismatched parentheses.
                    printf("RightParen error (%s)\n", token.str_.c_str());
                    errorString_ = "RightParen error!\n";
                    return {};
                }

                // Pop the left parenthesis from the stack,
                // but not onto the output queue.
                stack.pop_back();
            }
            break;

        default:
            printf("error (%s)\n", token.str_.c_str());
            errorString_ = ("Error (%s)\n", token.str_.c_str());
            return {};
        }
    }
    // When there are no more tokens to read:
    // While there are still operator tokens in the stack:
    while (!stack.empty()){
        // If the operator token on the top of the stack is a parenthesis,
        // then there are mismatched parentheses.
        if (stack.back().type_ == Token::Type::LeftParen){
            printf("Mismatched parentheses error\n");
            errorString_ = "Mismatched parentheses error";
            return {};
        }

        // Pop the operator onto the output queue.
        queue.push_back(std::move(stack.back()));
        stack.pop_back();
    }
    return queue;
}

void Tree::BFS(Node* node){
    if (node->value_.type_ == Token::Type::Number || node->value_.type_ == Token::Type::Matrix){
        return ;
    }
    if ((node->value_.type_ == Token::Type::Function || node->value_.type_ == Token::Type::Operator) &&
        (node->leftChild_->value_.type_ == Token::Type::Number || node->leftChild_->value_.type_ == Token::Type::Matrix) &&
        (node->rightChild_ == nullptr || node->rightChild_->value_.type_ == Token::Type::Number || node->rightChild_->value_.type_ == Token::Type::Matrix)){
        nodesCalc_.push_back(node);
        return ;
    }
    BFS(node->leftChild_);
    BFS(node->rightChild_);
}

void Tree::Compute(Node* node, std::stop_source& source){
    if (errorString_.size() != 0){
        return ;
    }
    std::stop_token stoken = source.get_token();
    if (stoken.stop_requested()){
        return ;
    }
    switch (node->value_.type_){
        case Token::Type::Operator:{
            if (node->value_.unary_ == 1){
                if (node->leftChild_->value_.type_ == Token::Type::Matrix){
                    auto lhs = matricies_[node->leftChild_->value_.str_];
                    switch (node->value_.str_[0]){
                    default:
                        printf("Operator error [%s]\n", node->value_.str_.c_str());
                        errorString_ = ("Operator error [%s]\n", node->value_.str_.c_str());
                        source.request_stop();
                        return ;
                    case 'm':  
                        matricies_[node->leftChild_->value_.str_] = -matricies_[node->leftChild_->value_.str_]; // Special operator name for unary '-'
                        break;
                    }
                    if (stoken.stop_requested()){
                        return ;
                    }
                    node->value_.str_ = node->leftChild_->value_.str_;
                    node->value_.type_ = Token::Type::Matrix;
                    node->AddChildren(nullptr);
                    break;
                }
                const auto lhs = stod(node->leftChild_->value_.str_);
                switch (node->value_.str_[0]){
                default:
                    printf("Operator error [%s]\n", node->value_.str_.c_str());
                    errorString_ = ("Operator error [%s]\n", node->value_.str_.c_str());
                    source.request_stop();
                    return ;
                case 'm':  
                    node->value_.str_ = std::to_string(-lhs); // Special operator name for unary '-'
                    break;
                }
                if (stoken.stop_requested()){
                    return ;
                }
                node->value_.type_ = Token::Type::Number;
                node->AddChildren(nullptr);
            } else {
                if (node->leftChild_->value_.type_ == Token::Type::Number && node->rightChild_->value_.type_ == Token::Type::Number){
                    const auto lhs = std::stod(node->leftChild_->value_.str_);
                    const auto rhs = std::stod(node->rightChild_->value_.str_);
                    switch(node->value_.str_[0]){
                    default:
                        printf("Operator error [%s]\n", node->value_.str_.c_str());
                        errorString_ = ("Operator error [%s]\n", node->value_.str_.c_str());
                        source.request_stop();
                        return ;
                    case '^':
                        node->value_.str_ = std::to_string(static_cast<int64_t>(pow(lhs, rhs)));
                        break;
                    case '*':
                        node->value_.str_ = std::to_string(lhs * rhs);
                        break;
                    case '/':
                        if (rhs == 0){
                            printf("Division by zero!\n");
                            errorString_ = "Division by zero!";
                            source.request_stop();
                            return ;
                        } 
                        node->value_.str_ = std::to_string(lhs / rhs);
                        break;
                    case '+':
                        node->value_.str_ = std::to_string(lhs + rhs);
                        break;
                    case '-':
                        node->value_.str_ = std::to_string(lhs - rhs);
                        break;
                    }
                    if (stoken.stop_requested()){
                        return ;
                    }
                    node->value_.type_ = Token::Type::Number;
                    node->AddChildren(nullptr, nullptr);
                } else if (node->leftChild_->value_.type_ == Token::Type::Number || node->rightChild_->value_.type_ == Token::Type::Number){
                    if (node->rightChild_->value_.type_ == Token::Type::Number){
                        std::swap(node->leftChild_, node->rightChild_);
                    }
                    const auto lhs = std::stod(node->leftChild_->value_.str_);
                    auto rhs = &matricies_[node->rightChild_->value_.str_];
                    switch (node->value_.str_[0]){
                    default:
                        printf("Operator error [%s]\n", node->value_.str_.c_str());
                        errorString_ = ("Operator error [%s]\n", node->value_.str_.c_str());
                        source.request_stop();
                        return ;
                    case '*':
                        *rhs *= lhs;
                        break;
                    case '/':
                        if (lhs == 0){
                            printf("Division by zero!\n");
                            errorString_ = "Division by zero!";
                            source.request_stop();
                            return ;
                        } 
                        *rhs /= lhs;
                        break;
                    }
                    if (stoken.stop_requested()){
                        return ;
                    }
                    node->value_.str_ = node->rightChild_->value_.str_;
                    node->value_.type_ = Token::Type::Matrix;
                    node->AddChildren(nullptr);
                    break;
                } else {
                    auto lhs = &matricies_[node->leftChild_->value_.str_];
                    auto rhs = &matricies_[node->rightChild_->value_.str_];
                    switch (node->value_.str_[0]) {
                    default:
                        printf("Operator error [%s]\n", node->value_.str_.c_str());
                        errorString_ = ("Operator error [%s]\n", node->value_.str_.c_str());
                        source.request_stop();
                        return ;
                    case '*':
                        if (lhs->size2() != rhs->size1()){
                            printf("Size of matricies are incorrect!\n");
                            errorString_ = ("Size of matricies are incorrect!");
                            source.request_stop();
                            return ;
                        } 
                        *lhs = boost::numeric::ublas::prod(*lhs, *rhs);
                        break;
                    case '+':
                        if (lhs->size1() != rhs->size1() || lhs->size2() != rhs->size2()){
                            printf("Size of matricies are incorrect!\n");
                            errorString_ = "Size of matricies are incorrect!";
                            source.request_stop();
                            return ;
                        } 
                        *lhs += *rhs;
                        break;
                    case '-':
                        if (lhs->size1() != rhs->size1() || lhs->size2() != rhs->size2()){
                            printf("Size of matricies are incorrect!\n");
                            errorString_ = "Size of matricies are incorrect!";
                            source.request_stop();
                            return ;
                        } 
                        *lhs -= *rhs;
                        break;
                    }
                    if (stoken.stop_requested()){
                        return ;
                    }
                    node->value_.str_ = node->leftChild_->value_.str_;
                    matricies_.erase(node->rightChild_->value_.str_);
                    node->value_.type_ = Token::Type::Matrix;
                    node->AddChildren(nullptr, nullptr);
                }
                
            }
            break;
        } case Token::Type::Function: {
            if (node->value_.unary_ == 1){
                const auto lhs = std::stod(node->leftChild_->value_.str_);
                node->value_.str_ = std::to_string(wrap_.map_functions[node->value_.str_].first(static_cast<double>(lhs), 0));
            } else {
                const auto lhs = std::stod(node->leftChild_->value_.str_);
                const auto rhs = std::stod(node->rightChild_->value_.str_);
                node->value_.str_ = std::to_string(wrap_.map_functions[node->value_.str_].first(static_cast<double>(lhs), static_cast<double>(rhs)));
            }
            if (stoken.stop_requested()){
                return ;
            }
            node->value_.type_ = Token::Type::Number;
            node->AddChildren(nullptr, nullptr);
            break;
        }
    }
}

std::string Tree::MultithreadCompute(){ //stop_source
    if (!errorString_.empty()){
        return errorString_;
    }
    using namespace std::chrono_literals;
    std::vector<std::jthread> threads;
    std::stop_source source;
    while ((root_->value_.type_ != Token::Type::Number) || (root_->value_.type_ != Token::Type::Matrix)){
        BFS(root_);
        if (nodesCalc_.empty()){
            break;
        }
        {
        for (Node* node: nodesCalc_){
            threads.emplace_back(&Tree::Compute, this, node, std::ref(source));
        }
        }
        if (!errorString_.empty()){
            return errorString_;
        }
        nodesCalc_.clear();
        threads.clear();
    }
    if (root_->value_.type_ == Token::Type::Matrix){
        std::string result;
        for (size_t i = 0; i < matricies_[root_->value_.str_].size1(); ++i){
            for (size_t j = 0; j < matricies_[root_->value_.str_].size2(); ++j){
                std::cout << "result elem: " << matricies_[root_->value_.str_](i, j) << '\n';
                result += std::to_string(matricies_[root_->value_.str_](i, j)) + ' ';
            }
            result += '\n';
        }
        return result;
    }
    return root_->value_.str_;
}

std::string Tree::GetErrorString(){
    return errorString_;
}
