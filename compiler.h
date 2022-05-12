#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Compiler
{
public:
    Compiler();
    void run(int argc, char *argv[]);
    void parse(string text, vector<string> local={});
    void error(string line, string info="Unknown Definition", string type="warning");
    string getVar(string var_name);
    void setVar(string var_name, string data);
    string getLocalVar(string var_name, vector<string> local);
    string getFunc(string func_name, bool getArgs=false);
    string parseExpr(string tok0, string tok1, string line, vector<string> local);
    void parseMulti(bool isFunc, string code, string rawName, vector<string> lines, int i);

    void print(string str);
    vector<string> split(string s, string delimiter);
    bool replace(std::string& str, const std::string& from, const std::string& to);
    void remove_item(std::string& str, vector<string> to_remove);
    std::string strip(const std::string &inpt);
    void check(vector<string>& vec, size_t count);
private:
    string io;
    int l;
    vector<string> vars;
    vector<string> funcs;
    bool multiLine;
};

#endif // COMPILER_H
