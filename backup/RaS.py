var = []
func = []

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''


def run(text, extra=[], filename="<stdin>"):
    def error():
        print(text)
        for c in text:
            print(f"{bcolors().FAIL}^{bcolors().ENDC}", end="")
        print()
        print(f"Unknown Definition in <{filename}>, " + "'" + tok[0] + "'")
        print()

    def getVar(var_name):
        for varObj in var:
            rawData = varObj.split("%^%")
            varName = rawData[0]
            varData = rawData[1]
            if "{" in var_name:
                if (var_name.split("{")[0] == varName):
                    i = int(var_name.split("{")[1].replace("}", ""))
                    data = varData.split(",")
                    res = data[i].replace("<", "").replace(">", "").strip()
                    if res[0] == "$":
                        res = getVar(res)
                    return res
            else:
                if var_name == varName:
                    return varData
        
    def getFunc(func_name):
        for f in func:
            rawData = f.split("&*&")
            varName = rawData[0]
            varData = rawData[1]
            if func_name == varName:
                return varData
            else:
                return "Not found"

    def getLocalVar(var_name, extra):
        if var_name[0] == "$":
            getVar(var_name)
        else:
            for varObj in extra:
                rawData = varObj.split("$")
                varName = rawData[0]
                varData = rawData[1]
                if "{" in var_name:
                    if (var_name.split("{")[0] == varName):
                        i = int(var_name.split("{")[1].replace("}", ""))
                        data = varData.split(",")
                        res = data[i].replace("<", "").replace(">", "").strip()
                        if res[0] == "$":
                            res = getVar(res)
                        return res
                else:
                    if var_name == varName:
                        return varData

    lines = text.split("\n")
    for line in lines:
        tok = line.split(" ")
        if tok[0] == "print":
            if tok[1][0] == "$":
               print(getVar(tok[1]))
            elif "this." in tok[1]:
                print(getLocalVar(tok[1], extra))
            elif tok[1][0] == "[":
                tokens = text.replace(tok[0], "").strip().split("+")
                data = ""
                for item in tokens:
                    obj = item.replace("[", "").replace("]", "")
                    if(obj[0] == "$"):
                        obj = getVar(obj)
                    if("this." in obj):
                        obj = getLocalVar(obj, extra)
                    data = data + obj
                print(data)
            else:
                print(text.replace(tok[0], "").strip())
        elif tok[0] == "var":
            if tok[2] == "=":
                if tok[3][0] == "{":
                    args = text.replace(tok[0], "").replace(tok[1], "").replace(tok[2], "").replace("{", "").replace("}", "").strip()
                    stdin = input(args + " ")
                    to_add = "$" + tok[1] + "%^%" + stdin
                    var.append(str(to_add))
                else:
                    args = text.replace(tok[0], "").replace(tok[1], "").replace(tok[2], "").strip()
                    to_add = "$" + tok[1] + "%^%" + args
                    var.append(str(to_add))
            else:
                print("Unexpected Char in <stdin>, " + "'" + tok[2] + "'")
        elif tok[0] == "each":
            var_item = tok[1]
            var_type = tok[2]
            var_array = tok[3].replace(":", "")
            code = text.split(var_array+":")[1].strip()
            if var_type != "->":
                print("Unexpected Char in <stdin>, " + "'" + var_type + "'")
            array = getVar(var_array).split(",")
            for item in array:
                data = item.replace("<", "").replace(">", "").strip()
                run(code, [ "this." + var_item + "$" + data ])
        elif tok[0] == "import":
            file_to_open = tok[1]
            dat = open(file_to_open, "r").read()
            run(dat)
        elif tok[0] == "while":
            test = text.split("->")[1].replace(":", "").strip()
            code = text.split(":")[1].strip()
            testing = test.replace(code, "").split("=")
            op1 = testing[0].strip()
            op2 = testing[1].strip()
            while(op1 == op2 or getVar(op1) == getVar(op2) or getVar(op1) == op2 or op1 == getVar(op2)):
                run(code)
        elif tok[0] == "each":
            var_item = tok[1]
            var_type = tok[2]
            var_array = tok[3].replace(":", "")
            code = text.split(var_array+":")[1].strip()
            if var_type != "->":
                print("Unexpected Char in <stdin>, " + "'" + var_type + "'")
            array = getVar(var_array).split(",")
            for item in array:
                data = item.replace("<", "").replace(">", "").strip()
                run(code, [ "this." + var_item + "$" + data ])
        elif tok[0] == "eval":
            eval(text.replace(tok[0], ""))
        elif tok[0] == "if":
            test = text.split("->")[1].replace(":", "").strip()
            code = text.split(":")[1].strip()
            testing = test.replace(code, "").split("=")
            op1 = testing[0].strip()
            op2 = testing[1].strip()
            if(op1 == op2 or getVar(op1) == getVar(op2) or getVar(op1) == op2 or op1 == getVar(op2)):
                run(code)
        elif tok[0] == "func":
            name = tok[1].replace(":", "")
            code = text.replace(tok[0], "").split(tok[1])[1].strip()
            data = name + "&*&" + code
            func.append(data)
        elif tok[0] == "#":
            pass
        elif tok[0] == "exit":
            exit("Program done.")
        elif getFunc(tok[0]) != "Not found":
            if getFunc(tok[0]) != None:
                run(getFunc(tok[0]), [], "func:app.ras")
            else:
                error()
        else:
           error()

# print("Welcome to Ras v1.0")
