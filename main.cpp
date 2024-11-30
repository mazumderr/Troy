#include <iostream>
#include "SymbolTable.hpp"
#include "DescentParser.hpp"
#include <string>

using namespace std;

CodeNode* Interpreter(CodeNode*,void* &,SymbolType);
int valueConversion(Token*);
CodeNode* getEndBlock(CodeNode* begin);
int assignment(CodeNode*, CodeScope*, bool&);

int main(int argc, char** argv){

  if (argc < 2) {
    cout << "usage: " << argv[0] << " inputfile" << endl;
    return 0;
  }

  void* returnValue;
  SyntaxTree* AST;
  CodeScope* symbolTable;
  DescentParser d;
  d.parse(argv[1],AST,symbolTable);
  Interpreter(symbolTable->lookupSymbol("main")->myDeclaration, returnValue, SymbolType::NONE);
  //okay now assume that argv[1] is the desired input filename :D

  return 0;
}

int valueConversion(Token* cur){
    if((cur->getType() == TokenType::STRING) && (cur->getSpelling().size() == 1)){
        return cur->getSpelling().at(0); // returns a char
    }
    else{
        return std::stoi(cur->getSpelling());
    }
}

/**
 * @brief Evaluate a mathematical or boolean expression.
 *
 * This function processes a vector of tokens to compute the result of an expression.
 * It supports arithmetic operators (+, -, *, /, %), comparison operators (==, !=, <, <=, >, >=),
 * and logical operators (&&, ||, !). It resolves variables from the given scope and handles
 * function or procedure calls in the expression.
 *
 * @param equation The vector of tokens representing the expression.
 * @param Scope Pointer to the current scope for symbol lookup.
 * @param isBool Boolean flag indicating if the expression is a boolean expression.
 * @return The result of the expression as an integer.
 */
int math(vector<Token> equation, CodeScope* Scope, bool &isBool){
    int value = 0;
    Symbol* varible; // Pointer for variable resolution.
    Symbol* varible2; // Pointer for secondary variable resolution.
    vector<Token> equationCopy; // Copy of the input expression for processing.

    // Create a copy of the input expression for safe modification.
    for(int i = 0; i < equation.size(); i++){
        equationCopy.push_back(equation.at(i));
    }

    // Process each token in the expression to evaluate it.
    for (int i = 0; i < equationCopy.size(); i++) {


//        Symbol*array = Scope->lookupSymbol(equationCopy.at(0).getSpelling());
//        if(array->isArray){
//            for(int j = )
//            array->value[i-2] =
//        }


        Token toke; // Temporary token for result storage.

        // Check if a function or procedure call is involved.
        if((equationCopy.at(i).getType() == TokenType::IDENTIFIER)
        && ((Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::FUNCTION)
        || Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::PROCEDURE)){

            // Resolve function/procedure call and replace with its result.
            CodeNode* FuncDef = Scope->lookupSymbol(equationCopy.at(i).getSpelling())->myDeclaration;
            SymbolType returnType = Scope->lookupSymbol(equationCopy.at(i).getSpelling())->returnType;
            Symbol* firstParam = FuncDef->getChild()->getScope()->getParameters().front();

            if(Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->isArray){
                int *arrayElement = Scope->lookupSymbol(equationCopy.at(i+3).getSpelling())->value;
                firstParam->value = &Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->value[*arrayElement];
            }
            else{
                firstParam->value = Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->value;
            }
            void* returnValue;
            if(Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::PROCEDURE){
                Interpreter(FuncDef,returnValue, SymbolType::NONE);
            }
            else {
                Interpreter(FuncDef, returnValue,returnType);
            }

            // Replace function call with its evaluated result in the expression.
            equationCopy.erase(equationCopy.begin() +(i+1));
            equationCopy.erase(equationCopy.begin()+i);

            toke.setSpelling(std::to_string(*((int*)returnValue))); //fix me
            toke.setType(TokenType::INTEGER);
            equationCopy.insert(equationCopy.begin()+i,toke);
            continue;
        }
        // Evaluate the expression based on the operator type.
        switch (equationCopy.at(i).getType()){
            //if it is Multiplication
            case TokenType::ASTERISK:
                // Multiplication

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    value = valueConversion(&equationCopy.at(i-1)) * valueConversion(&equationCopy.at(i-2));
                }

                //checking if the first int being multiplied is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //multiplying the 2 values together
                    value = varible->value[0] * valueConversion(&equationCopy.at(i-2));
                }

                //checking if the other int being multiplied is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->value[0] * valueConversion(&equationCopy.at(i-1));
                }
                //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible->value[0] * varible2->value[0];
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::PLUS:
                // Addition

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    value = valueConversion(&equationCopy.at(i-1)) + valueConversion(&equationCopy.at(i-2));
                }

                    //checking if the first int being multiplied is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //multiplying the 2 values together
                    value = varible->value[0] + valueConversion(&equationCopy.at(i-2));
                }

                    //checking if the other int being multiplied is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->value[0] + valueConversion(&equationCopy.at(i-1));
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible->value[0] + varible2->value[0];
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::MINUS:
                // Subtraction

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    value = valueConversion(&equationCopy.at(i-2)) - valueConversion(&equationCopy.at(i-1));
                }

                //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) - varible->value[0];
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->value[0] - valueConversion(&equationCopy.at(i-1));
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->value[0] - varible->value[0];
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::DIVIDE:

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    value = valueConversion(&equationCopy.at(i-2)) / valueConversion(&equationCopy.at(i-1));
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) / varible->value[0];
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->value[0] / valueConversion(&equationCopy.at(i-1));
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->value[0] / varible->value[0];
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::MODULO:

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    value = valueConversion(&equationCopy.at(i-2)) % valueConversion(&equationCopy.at(i-1));
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) % varible->value[0];
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->value[0] % valueConversion(&equationCopy.at(i-1));
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->value[0] % varible->value[0];
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::BOOLEAN_OR:
                isBool = true;
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if((valueConversion(&equationCopy.at(i-2)) == 1) || (valueConversion(&equationCopy.at(i-1)) == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if((valueConversion(&equationCopy.at(i-2)) == 1) || (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if((valueConversion(&equationCopy.at(i-1)) == 1) || (varible2->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if((varible2->value[0] == 1) || (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::BOOLEAN_AND:
                isBool = true;
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if(((valueConversion(&equationCopy.at(i-2))) == 1) && (valueConversion(&equationCopy.at(i-1)) == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if((valueConversion(&equationCopy.at(i-2)) == 1) && (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if((valueConversion(&equationCopy.at(i-1)) == 1) && (varible2->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if((varible2->value[0] == 1) && (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::BOOLEAN_EQUAL:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if(valueConversion(&equationCopy.at(i-2)) == valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) == varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // int == b
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(valueConversion(&equationCopy.at(i-1)) == varible2->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] == varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::GREATER_THAN_OR_EQUAL:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    if(valueConversion(&equationCopy.at(i-2)) >= valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) >= varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(varible2->value[0] >= valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] >= varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::GREATER_THAN:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    if(valueConversion(&equationCopy.at(i-2)) > valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) > varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // int == b
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(varible2->value[0] > valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] > varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::LESS_THAN_OR_EQUAL:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    if(valueConversion(&equationCopy.at(i-2)) <= valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) <= varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // int == b
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER || equationCopy.at(i-1).getType() == TokenType::STRING) && (equationCopy.at(i-2).getType() != TokenType::INTEGER || equationCopy.at(i-2).getType() != TokenType::STRING)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(varible2->value[0] <= valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] <= varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::LESS_THAN:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if(valueConversion(&equationCopy.at(i-2)) < valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) < varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // int == b
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(varible2->value[0] < valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] < varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::NOT_EQUAL:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if(valueConversion(&equationCopy.at(i-2)) != valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // a == int
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(valueConversion(&equationCopy.at(i-2)) != varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    // int == b
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    if(valueConversion(&equationCopy.at(i-1)) != varible2->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    if(varible2->value[0] != varible->value[0]){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                equationCopy.erase(equationCopy.begin()+(i-2));
                i -= 2;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

            case TokenType::NOT:
                isBool = true;
                // int == int
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER)){
                    if(valueConversion(&equationCopy.at(i-1)) == 0){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                else{
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    if(varible->value[0] == 0){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }
                //erasing the values from the vector and replace with the new value just calculated
                equationCopy.erase(equationCopy.begin() +i);
                equationCopy.erase(equationCopy.begin()+(i-1));
                i -= 1;
                toke.setSpelling(std::to_string(value));
                toke.setType(TokenType::INTEGER);
                equationCopy.insert(equationCopy.begin()+i,toke);
                break;

        }
    }
    // If only one token is left, return its value as the result.
    if(equationCopy.size() == 1){
        return valueConversion(&equationCopy.at(0));
    }
    return valueConversion(&equationCopy.at(1)); // Return the final computed value.
}

CodeNode* Interpreter(CodeNode* start, void* &returnValue, SymbolType returnType){
    CodeScope* curScope = start->getChild()->getScope();
    CodeNode* cur = start->getChild();
    bool needElse = true;
    //go until we hit the end of the program
    while((cur->getType() != NodeType::END)){
        bool curBool = false;
        switch (cur->getType()) {
            //if it is an assignment type we need to evaluate the expression
            case NodeType::ASSIGNMENT: {
                assignment(cur,curScope,curBool);
                cur = cur->getChild();
            }
            break;
            case NodeType::RETURN:
                returnValue = (void*)(new int(curScope->lookupSymbol(cur->getExpression().at(0).getSpelling())->value[0]));
                cur = cur->getChild();
                break;
            case NodeType::IF:
                if(assignment(cur,curScope,curBool) != 0 ){
                    cur = Interpreter(cur,returnValue,returnType)->getChild();
                    needElse = false;
                }
                else{
                    needElse = true;
                    cur = getEndBlock(cur->getChild())->getChild();
                }
                break;

            case NodeType::PRINTF:{
                int count = 1;
                for(int i = 0; i<cur->getExpression().at(0).getSpelling().size(); i++){

                    if(cur->getExpression().at(0).getSpelling().at(i) == '\\'){
                        if(cur->getExpression().at(0).getSpelling().at(i+1) == 'n'){
                            std::cout<<"\n";
                            i++;
                        }
                    }

                    else if(cur->getExpression().at(0).getSpelling().at(i) == '%'){
                        if(curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->isArray){
                            int j = 0;
                            while((curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->value[j] != '\\') || (curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->value[j+1] != 'x') || (curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->value[j+2] != '0')){
                                char element = curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->value[j];
                                std::cout<< element;
                                j++;
                            }
                            count++;
                            i++;
                        }
                        else{
                            int print = curScope->lookupSymbol(cur->getExpression().at(count).getSpelling())->value[0];
                            std::cout<<print;
                            count++;
                            i++;
                        }
                    }
                    else{
                        std::cout<< cur->getExpression().at(0).getSpelling().at(i);
                    }
                }
            }
                cur = cur->getChild();
                break;

            case NodeType::FOR_INITIAL:{
                CodeNode* condition = cur->getSibling();
                CodeNode* increment = condition->getSibling();

                assignment(cur,curScope,curBool);

                while(math(condition->getExpression(), curScope, curBool) != 0){
                    curBool = false;
                    Interpreter(increment,returnValue,returnType);
                    assignment(increment,curScope,curBool);
                }
                cur = getEndBlock(increment->getChild())->getChild();

            }
            break;

            case NodeType::WHILE:
                while (math(cur->getExpression(), curScope, curBool) != 0){
                    curBool = false;
                    Interpreter(cur,returnValue,returnType);
                }
                cur = getEndBlock(cur->getChild())->getChild();
                break;

            case NodeType::CALL:
                math(cur->getExpression(),curScope,curBool);
                cur = cur->getChild();
                break;
            case NodeType::ELSE:
                if(needElse){
                    cur = Interpreter(cur,returnValue,returnType)->getChild();
                    if(cur->getChild()->getType() == NodeType::RETURN){
                        cur = cur->getChild();
                    }
                }
                else{
                    cur = getEndBlock(cur->getChild());
                }
                break;



            default:
                cur = cur->getChild();
        }
    }
    return cur;
}

CodeNode* getEndBlock(CodeNode* begin){
    int counter = 0;
    while(counter > 1 || begin->getType() != NodeType::END){
        if(begin->getType() == NodeType::BEGIN){
            counter++;
        }
        if(begin->getType() == NodeType::END){
            counter--;
        }
        begin = begin->getChild();
    }
    return begin;
}

int assignment(CodeNode* cur, CodeScope* curScope, bool &isBool){
    int ifBool;
    int value;
    Symbol *LHSOperand = curScope->lookupSymbol(cur->getExpression().at(0).getSpelling());
    if (LHSOperand->type == SymbolType::INT) {
        //if the expression is only 3 ints long then (Ex. a 5 =) no math needed
        if (cur->getExpression().size() == 2) {
            LHSOperand->assign(std::stoi(cur->getExpression().at(1).getSpelling()));
        }
            //otherwise let's evaluate the expression
        else{
            value = math(cur->getExpression(), curScope, isBool);
            if(isBool){
                ifBool = value;
            }
            else{
                LHSOperand->assign(value);
            }
        }
    }
    else if(LHSOperand->type == SymbolType::BOOL){
        if (cur->getExpression().size() == 2) {
            if(cur->getExpression().at(1).getSpelling() == "true"){
                ifBool = true;
                LHSOperand->assign(true); // temp = true;
            }
            else{
                ifBool = false;
                LHSOperand->assign(false);
            }
        }
        else{
            value = math(cur->getExpression(), curScope, isBool);
            if(isBool){
                ifBool = value;
            }
        }
    }
    else{
        if (cur->getExpression().size() == 2 && !LHSOperand->isArray) {
            LHSOperand->assign(cur->getExpression().at(1).getSpelling()[0]);
        }
        else if(LHSOperand->isArray){
            if(LHSOperand->value != nullptr){
                delete[]LHSOperand;
            }
            LHSOperand->value = new int[LHSOperand->arraySize];
            for(int i = 0; i< cur->getExpression().at(1).getSpelling().size(); i++){
                LHSOperand->value[i] = cur->getExpression().at(1).getSpelling().at(i);
            }
        }
        else{
            value = math(cur->getExpression(), curScope, isBool);
            if(isBool){
                ifBool = value;
            }
            else{
                LHSOperand->assign(value);
            }
        }
    }
    if(isBool){
        return ifBool;
    }

    return LHSOperand->value[0];
}
