var = []

def run(text):
    lines = text.split("\n")
    for line in lines:
        tok = line.split(" ")
        if tok[0] == "print":
            if tok[1][0] == "$":
                for varObj in var:
                    rawData = varObj.split("%^%")
                    varName = rawData[0]
                    varData = rawData[1]
                    if tok[1] == varName:
                        print(varData)
            else:
                print(tok[1])
        elif tok[0] == "var":
            if tok[2] == "=":
                args = text.replace(tok[0], "").replace(tok[1], "").replace(tok[2], "").strip()
                to_add = "$" + tok[1] + "%^%" + args
                var.append(str(to_add))
            else:
                print("Unexpected Char in <stdin>, " + "'" + tok[2] + "'")
        elif tok[0] == "#":
            print("")
        elif tok[0] == "import":
            file_to_open = tok[1]
            dat = open(file_to_open, "r").read()
            run(dat)
        elif tok[0] == "chk" and tok[3] in "0123456789" and tok[2] == "==":
           if tok[1][0] == "$":
            for i in var:
                rData = i.split("%^%")
                vName = rData[0]
                vData = rData[1]
                if vData == tok[3]:
                    print("True")
                else:
                    print("False")
        elif tok[0] == "chk" and tok[1] in "0123456789" and tok[2] == "==":
           if tok[3][0] == "$":
            for i in var:
                rData = i.split("%^%")
                vName = rData[0]
                vData = rData[1]
                if vData == tok[1]:
                    print("True")
                else:
                    print("False")
        elif tok[0] == "chk" and tok[1] and tok[3] in "0123456789" and tok[2] == "==":
            if tok[1] == tok[3]:
                print("True")
            else:
                print("False")
        elif tok[0] == "chk" and tok[2] == "==":
           if tok[1][0] == "$":
            for i in var:
                rData = i.split("%^%")
                vName = rData[0]
                vData = rData[1]
                if vData == tok[3]:
                    print("True")
                else:
                    print("False")
        elif tok[0] == "chk" and tok[2] == "==":
           if tok[3][0] == "$":
            for i in var:
                rData = i.split("%^%")
                vName = rData[0]
                vData = rData[1]
                if vData == tok[1]:
                    print("True")
                else:
                    print("False")
        elif tok[0] == "chk" and tok[2] == "==":
            if tok[1] == tok[3]:
                print("True")
            else:
                print("False")
        elif tok[0] == "exit":
            exit("Program done.")
        else:
            print("Unknown Definition in <stdin>, " + "'" + tok[0] + "'")

print("Welcome to Ras v1.0")