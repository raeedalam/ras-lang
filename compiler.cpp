#include "compiler.h"
#include <fstream>

#define repeat(x) for(int i = x; i--;)

void Compiler::print(string str)
{
    cout << str << endl;
}

Compiler::Compiler()
    : multiLine(false)
{
    Compiler::io = "stdin";
    Compiler::l = 0;
}

vector<string> Compiler::split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));

    if(res.size() == 1 && s != "" && delimiter != "" && delimiter != "\n" && delimiter != " ") {
        // error("object dump: "+s, "Unable to split string with delimiter '" + delimiter + "'", "error");
        return vector<string>{s};
    }

    return res;
}

bool Compiler::replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos) {
        error(str, "Unable to remove the char '" + from + "' in string '" + str + "'", "error");
        return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
}

void Compiler::remove_item(std::string& str, vector<string> to_remove)
{
    for(const string &item : to_remove) {
        replace(str, item, "");
    }
    
}

std::string Compiler::strip(const std::string &inpt)
{
    auto start_it = inpt.begin();
    auto end_it = inpt.rbegin();
    while (std::isspace(*start_it))
        ++start_it;
    while (std::isspace(*end_it))
        ++end_it;
    return std::string(start_it, end_it.base());
}

void Compiler::error(string line, string info, string type)
{
    print(line);
    repeat(line.length()){
        printf("\033[91m^\033[0m");
    }
    print("\n[\033[91m" + type + "\033[0m] " + info + " in Line: " + to_string(l) + " <" + io + ">");

    if(type == "error") exit(1);
}

string Compiler::getVar(string var_name)
{
    for(const string &varObj : vars) {
        vector<string> rawData = split(varObj, "%^%");
        string varName = rawData[0];
        string varData = rawData[1];
        if(var_name.find("{") != std::string::npos) {
            if(split(var_name, "{")[0] == varName) {
                string index = split(var_name, "{")[1];
                replace(index, "}", "");
                __SIZE_TYPE__ i = atoi(index.c_str());
                vector<string> items = split(varData, ",");
                if(i > items.size()) error(var_name, "Array index out of bounds");
                string data = items[i];
                data = strip(data);
                if(data[0] == '$')
                    data = getVar(data);
                if (data[0] == '{') {
                    remove_item(data, { "{", "}" });
                    data = strip(data) + " ";
                    printf("%s", data.c_str());
                    string _stdin;
                    getline(cin, _stdin);
                    data = strip(_stdin);
                }
                return data;
            }
        }
        else if(var_name == varName) {
            return varData;
        }
    }
    error(var_name, "Unknown var");
    return "Not found";
}

void Compiler::setVar(string var_name, string data)
{
    for(size_t i=0; i < vars.size(); ++i) {
        string varObj = vars[i];
        vector<string> rawData = split(varObj, "%^%");
        string varName = rawData[0];
        string varData = rawData[1];
        if(var_name == varName) {
            vars[i] = varName + "%^%" + data;
        }
    }
}

string Compiler::getLocalVar(string var_name, vector<string> local)
{
    for(const string &varObj : local) {
        vector<string> rawData = split(varObj, "%^%");
        string varName = rawData[0];
        string varData = rawData[1];
        if(var_name == varName) {
            if(varData[0] == '$')
                return getVar(varData);
            if (varData[0] == '{') {
                remove_item(varData, { "{", "}" });
                varData = strip(varData) + " ";
                printf("%s", varData.c_str());
                string _stdin;
                getline(cin, _stdin);
                varData = strip(_stdin);
            }
            return varData;
        }
    }
    error(var_name, "Unknown local var");
    return "Not found";
}

string Compiler::getFunc(string func_name, bool getArgs)
{
    bool found = false;
    string result;

    for(const string &f : funcs) {
        vector<string> rawData = split(f, "&*&");
        string varName;

        if(rawData[0].find("~") != std::string::npos)
            varName = split(rawData[0], "~")[0];
        else
            varName = rawData[0];

        string varData = rawData[1];
        if(func_name == varName) {
            if(getArgs) result = split(rawData[0], "~")[1];
            else result = varData;
            found = true;
        }
    }

    if(found) {
        return result;
    } else {
        return "Not found";
    }
}

void Compiler::check(vector<string>& vec, size_t count)
{
    if(vec.size() < count) {
        error(vec[0], "Invalid amount of args: Wanted: " + to_string(count) + ", Got: " + to_string(vec.size()), "error");
    }
}

string Compiler::parseExpr(string tok0, string tok1, string line, vector<string> local)
{
    if(tok1[0] == '$') {
        return (getVar(tok1));
    } else if(tok1[0] == '*') {
        return (getLocalVar(tok1, local));
    } else if(tok1[0] == '[') {
        string _tokens = strip(line);
        replace(_tokens, tok0, "");
        vector<string> tokens = split(_tokens, "+");
        string data;
        for(const string& item : tokens) {
            string obj = strip(item);
            remove_item(obj, { "[", "]" });
            if(obj[0] == '$')
                obj = getVar(obj);
            if(obj[0] == '*')
                obj = getLocalVar(obj, local);
            if (obj[0] == '{') {
                remove_item(obj, { "{", "}" });
                obj = strip(obj) + " ";
                printf("%s", obj.c_str());
                string _stdin;
                getline(cin, _stdin);
                obj = strip(_stdin);
            }
            data = data + obj;
        }
        return (data);
    } else if (tok1[0] == '{') {
        string args = line;
        remove_item(args, { tok0, "{", "}" });
        args = strip(args) + " ";
        printf("%s", args.c_str());
        string _stdin;
        getline(cin, _stdin);
        return strip(_stdin);
    } else {
        string data = line;
        replace(data, tok0, "");
        return (strip(data));
    }
}

void Compiler::parseMulti(bool isFunc, string code, string rawName, vector<string> lines, int i)
{
    if(code == "<") {
        multiLine = true;
        string data = "";
        bool foundEnd = false;
        for(size_t j = (i+1); j <= lines.size(); j++) {
            string curLine = strip(lines[j]);
            if(curLine[0] == '>') {
                foundEnd = true;
                break;
            };
            data = data + curLine + "\n";
        }
        if(foundEnd){
            if(isFunc){
            string inject = rawName + "&*&" + data;
            funcs.push_back(inject);
            } else {
                multiLine = false;
                parse(data);
                multiLine = true;
            }
        } else {
            error(code, "Missing closing char. Try adding '>'?", "error");
        }
    } else {
        if(isFunc){
        string data = rawName + "&*&" + code;
        funcs.push_back(data);
        } else {
            multiLine = false;
            parse(code);
            multiLine = true;
        }
    }
}


void Compiler::run(int argc, char *argv[])
{
    if (argc > 1) {
        string file;
        file = argv[1];
        ifstream input(file);
        stringstream buffer;
        buffer << input.rdbuf();
        io = file;
        parse(buffer.str());
    } else {
        print("[Error] No file was given.");
    }
}

void Compiler::parse(string text, vector<string> local)
{
    vector<string> lines = split(text, "\n");

    for(size_t i = 0; i < lines.size(); ++i) {
        l++;
        string line = strip(lines[i]);
        vector<string> tok = split(line, " ");

        if(!multiLine) {
            if(tok[0] == "print") {
                check(tok, 2);
                string toPrint = parseExpr(tok[0], tok[1], line, local);
                print(toPrint);
            } else if(tok[0] == "var") {
                check(tok, 4);
                if(tok[2] == "=") {
                    if(tok[3][0] == '{') {
                        string args = line;
                        remove_item(args, { tok[0], tok[1], tok[2], "{", "}" });
                        args = strip(args) + " ";
                        printf("%s", args.c_str());
                        string _stdin;
                        getline(cin, _stdin);
                        string to_add = "$" + tok[1] + "%^%" + strip(_stdin);
                        vars.push_back(to_add);
                    } else {
                        string args = line;
                        remove_item(args, { tok[0], tok[1], tok[2] });
                        string to_add = "$" + tok[1] + "%^%" + strip(args);
                        vars.push_back(to_add);
                    }
                } else {
                    error(line, io);
                }
            } else if(tok[0] == "fn") {
                check(tok, 3);
                string rawName = tok[1];
                replace(rawName, ":", "");
                string code = line;
                replace(code, tok[0], "");
                code = strip(split(code, tok[1])[1]);
                parseMulti(true, code, rawName, lines, i);
            } else if(tok[0] == "if") {
                check(tok, 4);
                string test = line;
                replace(test, tok[0], "");
                replace(test, ":", "");
                test = strip(test);
                string code = split(line, ":")[1];
                code = strip(code);
                string data = test;
                replace(data, code, "");
                vector<string> testing = split(data, "=");
                check(testing, 1);
                string op1 = strip(testing[0]);
                string op2 = strip(testing[1]);
                if(op1[0] == '$'){
                    if(getVar(op1) == op2){
                        parseMulti(false, split(code, "<else>")[0], "", lines, i);
                    }
                } else if(op2[0] == '$'){
                    if(op1 == getVar(op2)){
                        parseMulti(false, split(code, "<else>")[0], "", lines, i);
                    }
                }
                else if(op1 == op2) {
                    parse(split(code, "<else>")[0]);
                } else {
                    vector<string> elseIfArr = split(code, "<else>");
                    string elseIfCode = strip(elseIfArr[1]);
                    parseMulti(false, elseIfCode, "", lines, i);
                }
                if(split(code, "<else>")[0] == "<") {
                    multiLine = true;
                }

            } else if(tok[0] == "each") {
                check(tok, 4);
                string var_item = tok[1];
                string var_type = tok[2];
                string var_array = tok[3];
                replace(var_array, ":", "");
                string code = split(line, var_array+":")[1];
                code = strip(code);
                if(var_type != "->") {
                    error(line, "Unexpected char, wanted '->', got '" + var_type + "'");
                }
                auto array = split(getVar(var_array), ",");
                for(const string& item : array) {
                    string data = item;
                    data = strip(data);
                    string args = "*" + var_item + "%^%" + data;
                    parse(code, { args });
                }
            } else if(tok[0] == "while") {
                check(tok, 4);
                string op_type = tok[3];
                string test = split(line, "->")[1];
                replace(test, ":", "");
                test = strip(test);
                string code = split(line, ":")[1];
                code = strip(code);
                string data = test;
                replace(data, code, "");

                if(op_type == "=") {
                    vector<string> testing = split(data, "=");
                    check(testing, 1);
                    string op1 = strip(testing[0]);
                    string op2 = strip(testing[1]);
                    if(op1[0] == '$'){
                        while(getVar(op1) == op2){
                            parse(code);
                        }
                    } else if(op2[0] == '$'){
                        while(op1 == getVar(op2)){
                            parse(code);
                        }
                    }
                    while(op1 == op2) {
                        parse(code);
                    }
                } else if(op_type == "!="){
                    vector<string> testing = split(data, "!=");
                    check(testing, 1);
                    string op1 = strip(testing[0]);
                    string op2 = strip(testing[1]);
                    if(op1[0] == '$'){
                        while(getVar(op1) != op2){
                            parse(code);
                        }
                    } else if(op2[0] == '$'){
                        while(op1 != getVar(op2)){
                            parse(code);
                        }
                    }
                    while(op1 != op2) {
                        parse(code);
                    }
                } else {
                    error(line, "Unknown char!!");
                }

            } else if(tok[0] == "exec") {
                check(tok, 2);
                string res = parseExpr(tok[0], tok[1], line, local);
                system(res.c_str());
            } else if(tok[0] == "exit") {
                exit(0);
            } else if(tok[0] == "import") {
                string data = line;
                replace(data, tok[0], "");
                data = strip(data) + ".ras";
                ifstream input(data);
                stringstream buffer;
                buffer << input.rdbuf();
                parse(buffer.str());
            } else if(tok[0] == "#") {
                // Comment
            } else if(tok[0] == "") {
                // Empty line
            } else if(tok[0][0] == '$') {
                check(tok, 2);
                setVar(tok[0], tok[2]);
            } else if(getFunc(tok[0]) != "Not found") {
                string argsToPass;
                string data = line;
                replace(data, tok[0], "");
                argsToPass = (strip(data).c_str());

                vector<string> passableArgs = {};

                if(argsToPass.find(",") != std::string::npos) {
                    vector<string> argsData = split(argsToPass, ",");
                    vector<string> argNames = split(getFunc(tok[0], true), ",");
                    for(__SIZE_TYPE__ i = 0; i < argNames.size(); ++i) {

                        string argDatum = argsData[i];
                        argDatum = parseExpr("", argsData[i], argDatum, local);

                        string args = string("*") + argNames[i] + string("%^%") + argDatum;
                        passableArgs.push_back(args);
                    }
                } else if(argsToPass != "") {
                    string args = string("*") + getFunc(tok[0], true) + string("%^%") + argsToPass;
                    passableArgs.push_back(args);

                }
                parse(getFunc(tok[0]), passableArgs);
            } else {
                error(line);
            }
        }
        if(multiLine && tok[0] == ">") multiLine = false;
    }
}
