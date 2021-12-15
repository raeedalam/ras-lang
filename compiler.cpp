#include "compiler.h"

#define repeat(x) for(int i = x; i--;)

void Compiler::print(string str)
{
    cout << str << endl;
}

Compiler::Compiler()
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
        error("object dump: "+s, "Unable to split string with delimiter '" + delimiter + "'", "error");
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
                uint i = atoi(index.c_str());
                vector<string> items = split(varData, ",");
                if(i > items.size()) error(var_name, "Array index out of bounds");
                string data = items[i];
                data = strip(data);
                if(data[0] == '$')
                    data = getVar(data);
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

string Compiler::getLocalVar(string var_name, vector<string> local)
{
    for(const string &varObj : local) {
        vector<string> rawData = split(varObj, "%^%");
        string varName = rawData[0];
        string varData = rawData[1];
        if(var_name == varName) {
            if(varData[0] == '$')
                return getVar(varData);
            return varData;
        }
    }
    error(var_name, "Unknown local var");
    return "Not found";
}

string Compiler::getFunc(string func_name)
{
    bool found = false;
    string result;

    for(const string &f : funcs) {
        vector<string> rawData = split(f, "&*&");
        string varName = rawData[0];
        string varData = rawData[1];
        if(func_name == varName) {
            result = varData;
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

    for(const string &line : lines) {
        l++;
        vector<string> tok = split(line, " ");

        if(tok[0] == "print") {
            check(tok, 2);
            if(tok[1][0] == '$') {
                print(getVar(tok[1]));
            } else if(tok[1][0] == '*') {
                print(getLocalVar(tok[1], local));
            } else if(tok[1][0] == '[') {
                string _tokens = strip(line);
                replace(_tokens, tok[0], "");
                vector<string> tokens = split(_tokens, "+");
                string data;
                for(const string& item : tokens) {
                    string obj = strip(item);
                    remove_item(obj, { "[", "]" });
                    if(obj[0] == '$')
                        obj = getVar(obj);
                    data = data + obj;
                }
                print(data);
            } else {
                string data = line;
                replace(data, tok[0], "");
                print(strip(data));
            }
        } else if(tok[0] == "var") {
            check(tok, 4);
            if(tok[2] == "=") {
                if(tok[3][0] == '{') {
                    string args = line;
                    remove_item(args, { tok[0], tok[1], tok[2], "{", "}" });
                    args = strip(args) + " ";
                    printf("%s", args.c_str());
                    string stdin;
                    getline(cin, stdin);
                    string to_add = "$" + tok[1] + "%^%" + strip(stdin);
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
        } else if(tok[0] == "func") {
            check(tok, 3);
            string name = tok[1];
            replace(name, ":", "");
            string code = line;
            replace(code, tok[0], "");
            code = strip(split(code, tok[1])[1]);
            string data = name + "&*&" + code;
            funcs.push_back(data);
        } else if(tok[0] == "if") {
            check(tok, 4);
            string test = split(line, "->")[1];
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
                    parse(code);
                }
            } else if(op2[0] == '$'){
                if(op1 == getVar(op2)){
                    parse(code);
                }
            }
            else if(op1 == op2) {
                parse(code);
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
        } else if(tok[0] == "exit") {
            exit(0);
        } else if(tok[0] == "#") {
            // Comment
        } else if(tok[0] == "") {
            // Empty line
        } else if(getFunc(tok[0]) != "Not found") {
            parse(getFunc(tok[0]));
        } else {
            error(line);
        }
    }
}
