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
  return 0;
}

/**
 * @brief Convert a token's value into an integer representation.
 *
 * This function checks the type of a given token and converts its value to an integer.
 * If the token represents a single-character string, it returns the ASCII value of the character.
 * Otherwise, it interprets the token's value as an integer string and converts it accordingly.
 *
 * @param cur Pointer to the token whose value needs to be converted.
 * @return The integer representation of the token's value.
 */
int valueConversion(Token* cur){
    // Check if the token is a single-character string.
    if((cur->getType() == TokenType::STRING) && (cur->getSpelling().size() == 1)){
        return cur->getSpelling().at(0); // Return the ASCII value of the character.
    }
    else{
        // Convert the string representation of the token to an integer.
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

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value = varible->value[0] + valueConversion(&equationCopy.at(i-2));
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
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

                //checking if the first int being subtracted is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER || equationCopy.at(i-1).getType() != TokenType::STRING) && (equationCopy.at(i-2).getType() == TokenType::INTEGER || equationCopy.at(i-2).getType() == TokenType::STRING)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) - varible->value[0];
                }

                    //checking if the other int being subtracted is a variable and if so then grabbing it from the symbol table
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

                    //checking if the first int being divided is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) / varible->value[0];
                }

                    //checking if the other int being divided is a variable and if so then grabbing it from the symbol table
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

                    //checking if the first int being Mod is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  valueConversion(&equationCopy.at(i-2)) % varible->value[0];
                }

                    //checking if the other int being Mod is a variable and if so then grabbing it from the symbol table
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

                    //checking if the first int being compared is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //comparing the 2 values together
                    if((valueConversion(&equationCopy.at(i-2)) == 1) || (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the other int being compared is a variable and if so then grabbing it from the symbol table
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
                    //otherwise both are variables and need to be looked up in the symbol table
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

                    //checking if the first int being compared is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //comparing the 2 values together
                    if((valueConversion(&equationCopy.at(i-2)) == 1) && (varible->value[0] == 1)){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

                    //checking if the other int being compared is a variable and if so then grabbing it from the symbol table
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
                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    if(valueConversion(&equationCopy.at(i-2)) == valueConversion(&equationCopy.at(i-1))){
                        value = 1;
                    }
                    else{
                        value = 0;
                    }
                }

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

            default: break;

        }
    }
    //If only one token is left, return its value as the result.
    if(equationCopy.size() == 1){
        return valueConversion(&equationCopy.at(0));
    }
    return valueConversion(&equationCopy.at(1)); // Return the final computed value.
}

/**
 * @brief Interpret and execute a syntax tree starting from a given node.
 *
 * This function traverses the syntax tree, executing various node types such as assignments,
 * loops, conditional statements, and function calls. It also handles return values for
 * functions or procedures and manages the scope during interpretation.
 *
 * @param start Pointer to the starting node in the syntax tree.
 * @param returnValue Reference to store the return value (if any) of the interpretation.
 * @param returnType The expected type of the return value.
 * @return A pointer to the next node after the current block of execution.
 */
CodeNode* Interpreter(CodeNode* start, void* &returnValue, SymbolType returnType){
    CodeScope* curScope = start->getChild()->getScope(); // Current scope for symbol resolution.
    CodeNode* cur = start->getChild();                   // Pointer to the current node being executed.
    bool needElse = true;                                // Flag to track whether to execute an ELSE block.
    // Traverse the syntax tree until the END node is reached.
    while((cur->getType() != NodeType::END)){
        bool curBool = false; // Tracks if the result of a condition is boolean.
        switch (cur->getType()) {
            //if it is an assignment type we need to evaluate the expression
            case NodeType::ASSIGNMENT: {
                // Handle an assignment operation.
                assignment(cur,curScope,curBool); // Perform the assignment.
                cur = cur->getChild();  // Move to the next node.
            }
            break;
            case NodeType::RETURN:
                // Handle a return statement.
                returnValue = (void*)(new int(curScope->lookupSymbol(cur->getExpression().at(0).getSpelling())->value[0])); //Saving the return value from the function
                cur = cur->getChild(); // Move to the next node.
                break;
            case NodeType::IF:
                // Handle an IF conditional statement.
                if(assignment(cur,curScope,curBool) != 0 ){
                    // Execute the IF block if the condition is true.
                    cur = Interpreter(cur,returnValue,returnType)->getChild();
                    needElse = false; // Skip the ELSE block.
                }
                else{
                    // Skip the IF block and move to the ELSE block (if present).
                    needElse = true;
                    cur = getEndBlock(cur->getChild())->getChild();
                }
                break;

            case NodeType::PRINTF:{
                //Handle a PRINTF statement.
                int count = 1; //Counter for handling format specifiers.
                for(int i = 0; i<cur->getExpression().at(0).getSpelling().size(); i++){

                    if(cur->getExpression().at(0).getSpelling().at(i) == '\\'){
                        if(cur->getExpression().at(0).getSpelling().at(i+1) == 'n'){
                            std::cout<<"\n";
                            i++;
                        }
                    }

                    else if(cur->getExpression().at(0).getSpelling().at(i) == '%'){
                        // Handle format specifiers.
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
                        std::cout<< cur->getExpression().at(0).getSpelling().at(i); //Print the character.
                    }
                }
            }
                cur = cur->getChild();
                break;

            case NodeType::FOR_INITIAL:{
                // Handle a FOR loop.
                CodeNode* condition = cur->getSibling(); // Condition node.
                CodeNode* increment = condition->getSibling(); // Increment node.

                assignment(cur,curScope,curBool); // Initialize the loop.

                // Execute the loop while the condition is true
                while(math(condition->getExpression(), curScope, curBool) != 0){
                    curBool = false; // Reset the boolean flag.
                    Interpreter(increment,returnValue,returnType); // Execute the loop body.
                    assignment(increment,curScope,curBool); // Perform the increment.
                }
                cur = getEndBlock(increment->getChild())->getChild(); // Skip to the end of the loop.

            }
            break;

            case NodeType::WHILE:
                // Handle a WHILE loop.
                while (math(cur->getExpression(), curScope, curBool) != 0){
                    curBool = false; // Reset the boolean flag.
                    Interpreter(cur,returnValue,returnType);
                }
                cur = getEndBlock(cur->getChild())->getChild(); // Skip to the end of the loop.
                break;

            case NodeType::CALL:
                // Handle a function or procedure call.
                math(cur->getExpression(),curScope,curBool); // Evaluate the call.
                cur = cur->getChild(); // Move to the next node.
                break;
            case NodeType::ELSE:
                // Handle an ELSE block.
                if(needElse){
                    cur = Interpreter(cur,returnValue,returnType)->getChild();
                    if(cur->getChild()->getType() == NodeType::RETURN){
                        cur = cur->getChild(); // Move to the RETURN node if present.
                    }
                }
                else{
                    cur = getEndBlock(cur->getChild());  // Skip the ELSE block.
                }
                break;



            default:
                // Handle other node types by moving to the child node.
                cur = cur->getChild();
        }
    }
    return cur;
}

/**
 * @brief Locate the END block node in a syntax tree.
 *
 * This function traverses the syntax tree starting from a given node to find the corresponding
 * END node of the current block. It ensures that nested blocks are properly handled by
 * tracking BEGIN and END nodes to maintain balance.
 *
 * @param begin Pointer to the starting node in the syntax tree (typically a BEGIN node).
 * @return A pointer to the END node of the block.
 */
CodeNode* getEndBlock(CodeNode* begin){
    int counter = 0; // Counter to track the depth of nested blocks.

    // Traverse the syntax tree until the correct END node is found.
    while(counter > 1 || begin->getType() != NodeType::END){
        if(begin->getType() == NodeType::BEGIN){
            counter++; // Entering a new nested block.
        }
        if(begin->getType() == NodeType::END){
            counter--;  // Exiting a nested block.
        }
        begin = begin->getChild(); // Move to the next node.
    }
    return begin; // Return the END node of the block.
}

/**
 * @brief Execute an assignment operation within the syntax tree.
 *
 * This function processes an assignment node, resolves the left-hand side (LHS) operand,
 * and evaluates the right-hand side (RHS) expression. It assigns the computed value
 * to the variable in the current scope, handling integer, boolean, and character types.
 * If the variable is an array, it assigns the appropriate values.
 *
 * @param cur Pointer to the current assignment node in the syntax tree.
 * @param curScope Pointer to the current scope for variable resolution.
 * @param isBool Reference to a boolean flag indicating if the result is a boolean value.
 * @return The result of the assignment as an integer, or the boolean value for conditional statements.
 */
int assignment(CodeNode* cur, CodeScope* curScope, bool &isBool){
    int ifBool; // Stores the result if the assignment is part of a conditional statement.
    int value; // Stores the computed value of the right-hand side expression.

    // Resolve the left-hand side (LHS) variable in the current scope.
    Symbol *LHSOperand = curScope->lookupSymbol(cur->getExpression().at(0).getSpelling());

    // Handle integer type assignments.
    if (LHSOperand->type == SymbolType::INT) {
        // Simple assignment: directly assign the value if the expression has only two tokens (e.g., a = 5).
        if (cur->getExpression().size() == 2) {
            LHSOperand->assign(std::stoi(cur->getExpression().at(1).getSpelling()));
        }
            // Evaluate the right-hand side expression and assign the result.
        else{
            value = math(cur->getExpression(), curScope, isBool);
            if(isBool){
                ifBool = value; // Store the boolean result for conditional evaluation.
            }
            else{
                LHSOperand->assign(value); // Assign the computed value.
            }
        }
    }
    // Handle boolean type assignments.
    else if(LHSOperand->type == SymbolType::BOOL){
        if (cur->getExpression().size() == 2) {
            // Assign a boolean literal (true/false).
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
            // Evaluate the right-hand side expression and assign the boolean result.
            value = math(cur->getExpression(), curScope, isBool);
            if(isBool){
                ifBool = value;
            }
        }
    }
    // Handle character type or array assignments.
    else{
        if (cur->getExpression().size() == 2 && !LHSOperand->isArray) {
            // Assign a single character.
            LHSOperand->assign(cur->getExpression().at(1).getSpelling()[0]);
        }
        else if(LHSOperand->isArray){
            if(LHSOperand->value != nullptr){
                delete[]LHSOperand; // Free the previous array memory.
            }
            // Assign values to an array.
            LHSOperand->value = new int[LHSOperand->arraySize];
            for(int i = 0; i< cur->getExpression().at(1).getSpelling().size(); i++){
                LHSOperand->value[i] = cur->getExpression().at(1).getSpelling().at(i);
            }
        }
        else{
            // Evaluate the right-hand side expression and assign the result.
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
        return ifBool; // Return the boolean value if the assignment is part of a conditional statement.
    }

    return LHSOperand->value[0]; // Return the final assigned value.
}
