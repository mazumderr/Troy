#include <iostream>
#include "SymbolTable.hpp"
#include "DescentParser.hpp"

using namespace std;

CodeNode* Interpreter(CodeNode*,void* &,SymbolType);

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

int math(vector<Token> equation, CodeScope* Scope, bool isBool){
    int value = 0;
    Symbol* varible;
    Symbol* varible2;
    vector<Token> equationCopy;
    for(int i = 0; i < equation.size(); i++){
        equationCopy.push_back(equation.at(i));
    }
    for (int i = 1; i < equationCopy.size(); i++) {
        Token toke;
        if((equationCopy.at(i).getType() == TokenType::IDENTIFIER) && ((Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::FUNCTION) || Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::PROCEDURE)){
            CodeNode* FuncDef = Scope->lookupSymbol(equation.at(i).getSpelling())->myDeclaration;
            SymbolType returnType = Scope->lookupSymbol(equationCopy.at(i).getSpelling())->returnType;
            Symbol* firstParam = FuncDef->getChild()->getScope()->getParameters().front();

            if(firstParam->type == SymbolType::INT){
                firstParam->myInt = Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->myInt;
            }
            if(firstParam->type == SymbolType::BOOL){
                firstParam->myBool = Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->myBool;
            }
            else{
                firstParam->myChar = Scope->lookupSymbol(equationCopy.at(i+1).getSpelling())->myChar;
            }
            void* returnValue;
            if(Scope->lookupSymbol(equationCopy.at(i).getSpelling())->type == SymbolType::PROCEDURE){
                Interpreter(FuncDef,returnValue, SymbolType::NONE);
            }
            else {
                Interpreter(FuncDef, returnValue,returnType);
            }
            equationCopy.erase(equationCopy.begin()+i);
            equationCopy.erase(equationCopy.begin() +(i+1));

            toke.setSpelling(std::to_string(*((int*)returnValue))); //fix me
            toke.setType(TokenType::INTEGER);
            equationCopy.insert(equationCopy.begin()+i,toke);
            continue;
        }
        //checking what math is needed to be done
        switch (equationCopy.at(i).getType()){
            //if it is Multiplication
            case TokenType::ASTERISK:

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    value = std::stoi(equationCopy.at(i-1).getSpelling()) * std::stoi(equationCopy.at(i-2).getSpelling());
                }

                //checking if the first int being multiplied is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //multiplying the 2 values together
                    value = varible->myInt * std::stoi(equationCopy.at(i-2).getSpelling());
                }

                //checking if the other int being multiplied is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->myInt * std::stoi(equationCopy.at(i-1).getSpelling());
                }
                //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible->myInt * varible2->myInt;
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

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    value = std::stoi(equationCopy.at(i-1).getSpelling()) + std::stoi(equationCopy.at(i-2).getSpelling());
                }

                    //checking if the first int being multiplied is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //multiplying the 2 values together
                    value = varible->myInt + std::stoi(equationCopy.at(i-2).getSpelling());
                }

                    //checking if the other int being multiplied is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->myInt + std::stoi(equationCopy.at(i-1).getSpelling());
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible->myInt + varible2->myInt;
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

                if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){
                    value = std::stoi(equationCopy.at(i-2).getSpelling()) - std::stoi(equationCopy.at(i-1).getSpelling());
                }

                //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  std::stoi(equationCopy.at(i-2).getSpelling()) - varible->myInt;
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->myInt - std::stoi(equationCopy.at(i-1).getSpelling());
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->myInt - varible->myInt;
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
                    value = std::stoi(equationCopy.at(i-2).getSpelling()) / std::stoi(equationCopy.at(i-1).getSpelling());
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  std::stoi(equationCopy.at(i-2).getSpelling()) / varible->myInt;
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->myInt / std::stoi(equationCopy.at(i-1).getSpelling());
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->myInt / varible->myInt;
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
                    value = std::stoi(equationCopy.at(i-2).getSpelling()) % std::stoi(equationCopy.at(i-1).getSpelling());
                }

                    //checking if the first int being added is an integer and the second is a variable
                else if((equationCopy.at(i-1).getType() != TokenType::INTEGER) && (equationCopy.at(i-2).getType() == TokenType::INTEGER)){

                    //getting the value saved in the symbol table
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());

                    //adding the 2 values together
                    value =  std::stoi(equationCopy.at(i-2).getSpelling()) % varible->myInt;
                }

                    //checking if the other int being added is a variable and if so then grabbing it from the symbol table
                else if((equationCopy.at(i-1).getType() == TokenType::INTEGER) && (equationCopy.at(i-2).getType() != TokenType::INTEGER)){

                    //grabbing value from symbol table
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());

                    //doing the math
                    value = varible2->myInt % std::stoi(equationCopy.at(i-1).getSpelling());
                }
                    //otherwise both ints are variables and need to be looked up in the symbol table
                else{
                    varible = Scope->lookupSymbol(equationCopy.at(i-1).getSpelling());
                    varible2 = Scope->lookupSymbol(equationCopy.at(i-2).getSpelling());
                    //doing the math
                    value  = varible2->myInt % varible->myInt;
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
        }
    }
    return std::stoi(equationCopy.at(1).getSpelling());
}

CodeNode* Interpreter(CodeNode* start, void* &returnValue, SymbolType returnType){
    CodeScope* curScope = start->getChild()->getScope();
    CodeNode* cur = start->getChild();
    std::cout<< "into loop we go" << "\n";
    //go until we hit the end of the program
    while(cur->getType() != NodeType::END){
        bool isBool = false;
        switch (cur->getType()) {
            //if it is an assignment type we need to evaluate the expression
            case NodeType::ASSIGNMENT: {
                Symbol *LHSOperand = curScope->lookupSymbol(cur->getExpression().at(0).getSpelling());
                if (LHSOperand->type == SymbolType::INT) {

                    //if the expression is only 3 ints long then (Ex. a 5 =) no math needed
                    if (cur->getExpression().size() == 2) {
                        LHSOperand->myInt = std::stoi(cur->getExpression().at(1).getSpelling());
                        std::cout << LHSOperand->myInt;
                    }
                    //otherwise let's evaluate the expression
                    else{
                        LHSOperand->myInt = math(cur->getExpression(), curScope, isBool);
                    }
                }
                cur = cur->getChild();
            }
            break;
            case NodeType::RETURN:
                switch(returnType){
                    case SymbolType::INT:
                        returnValue = (void*)(new int(curScope->lookupSymbol(cur->getExpression().at(0).getSpelling())->myInt));
                        break;
                    case SymbolType::BOOL:
                        returnValue = (void*)(new bool(curScope->lookupSymbol(cur->getExpression().at(0).getSpelling())->myBool));
                        break;
                    case SymbolType::CHAR:
                        returnValue = (void*)(new bool(curScope->lookupSymbol(cur->getExpression().at(0).getSpelling())->myChar));
                        break;
                    default:
                }
                cur = cur->getChild();
                break;
            case NodeType::IF:
                cur = Interpreter(cur,returnValue,returnType)->getChild(); //fix me
                break;
            default:
                cur = cur->getChild();
        }
    }
    return cur;
}