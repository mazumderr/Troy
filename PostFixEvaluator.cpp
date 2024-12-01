//
// Created by Joshua Lozano on 10/23/24.
//

#include "PostFixEvaluator.hpp"

PostFixEvaluator::PostFixEvaluator() {
    parenthesis = 0;
}

/**
 * @brief Evaluate a token and construct a postfix equation.
 *
 * This function processes a single token and updates the postfix equation. Depending on
 * the token type, it may directly append the token to the postfix equation, handle operators,
 * or manage parenthesis/bracket balancing for correct postfix notation.
 *
 * @param Symbol The token to evaluate and process.
 * @return A boolean indicating whether the evaluation was successful.
 */
bool PostFixEvaluator::Eval(Token Symbol) {
    switch (Symbol.getType()){
        // Handle an integer token.
        case TokenType::INTEGER:
            // If the previous token was also an integer, determine if subtraction or addition is implied.
            if(prev.getType() == TokenType::INTEGER) { // want subtraction to happen
                if(!isdigit(Symbol.getSpelling()[0])){
                    Token toke;
                    toke.setSpelling(string{Symbol.getSpelling()[0]}); // Extract sign from the integer.
                    toke.setType(Symbol.getSpelling()[0] == '+' ?
                                 TokenType::PLUS
                                                                :
                                 TokenType::MINUS);
                    Eval(toke); // Evaluate the operator token.
                    Symbol.getSpelling().erase(Symbol.getSpelling().begin()); // Remove the sign from the integer.
                }
            }
            // Fall through to handle appending to the postfix equation.
        case TokenType::STRING:
        case TokenType::IDENTIFIER:
            // Append identifiers, strings, or integers to the postfix equation.
            prev = Symbol;
            PostfixEquation.push_back(Symbol);
            return true;
        case TokenType::DOUBLE_QUOTE:
        case TokenType::SINGLE_QUOTE:
            // Double and single quotes are ignored for evaluation.
            return true;
        case TokenType::LEFT_BRACKET:
            // Handle a left bracket ([).
            prev = Symbol;
            PostfixEquation.push_back(Symbol);
            // Fall through to handle stacking.
        case TokenType::LEFT_PARENTHESIS:
            // Handle a left parenthesis or bracket.
            if(Symbol.getType() == TokenType::LEFT_BRACKET){
                Brackets++;
            }
            else{
                parenthesis++;
            }
            prev = Symbol;
            Stack.push(Symbol); // Push onto the operator stack.
            return true;
        case TokenType::RIGHT_BRACKET:
            // Handle a right bracket (]).
            while(Stack.top().getSpelling() != "["){
                PostfixEquation.push_back(Stack.top());
                Stack.pop();
            }
            PostfixEquation.push_back(Symbol); // Append the closing bracket.
            if(Brackets == 0){
                cerr << ("Too many closed Brackets");
                exit(1); // Exit if brackets are unbalanced.
            }
            Brackets--;
            Stack.pop(); // Pop the corresponding left bracket.
            return true;
        case TokenType::RIGHT_PARENTHESIS:
            // Handle a right parenthesis.
            if(parenthesis == 0){
                // If parenthesis are unbalanced, flush the stack to the postfix equation.
              while(!Stack.empty()){
                  PostfixEquation.push_back(Stack.top());
                  Stack.pop();
              }
              return false; // Indicate unbalanced parentheses.
            }
            parenthesis--;
            while(Stack.top().getSpelling() != "("){
                PostfixEquation.push_back(Stack.top());
                Stack.pop(); // Pop the corresponding left parenthesis.
            }
            Stack.pop();
            return true;

            // Handle operators with precedence (e.g., +, -, *, /, &&, ||, etc.).
        case TokenType::NOT:
        case TokenType::ASTERISK:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::MINUS:
        case TokenType::PLUS:
        case TokenType::GREATER_THAN:
        case TokenType::LESS_THAN:
        case TokenType::GREATER_THAN_OR_EQUAL:
        case TokenType::LESS_THAN_OR_EQUAL:
        case TokenType::BOOLEAN_EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::BOOLEAN_AND:
        case TokenType::BOOLEAN_OR:
            // Push the operator onto the stack based on precedence.
            if(Stack.empty() || (Precedence[Stack.top().getSpelling()] < Precedence[Symbol.getSpelling()])){
                prev = Symbol;
                Stack.push(Symbol);
            }
            else if(Precedence[Stack.top().getSpelling()] == Precedence[Symbol.getSpelling()]){
                PostfixEquation.push_back(Stack.top());
                Stack.pop();
                prev = Symbol;
                Stack.push(Symbol);
            }
            else{
                // Pop higher precedence operators from the stack.
                while(!Stack.empty() && (Precedence[Stack.top().getSpelling()] > Precedence[Symbol.getSpelling()])){
                    PostfixEquation.push_back(Stack.top());
                    prev = Symbol;
                    Stack.pop();
                }
                prev = Symbol;
                Stack.push(Symbol);
            }
            return true;
        default:
            // Flush the remaining stack to the postfix equation for unsupported tokens.
            while(!Stack.empty()){
                PostfixEquation.push_back(Stack.top());
                Stack.pop();
            }
            return false; // Indicate end of evaluation.
    }
}

vector<Token> PostFixEvaluator::getEquation() {
    return PostfixEquation;
}