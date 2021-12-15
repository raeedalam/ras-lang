#include <iostream>
#include "compiler.h"

int main(int argc, char *argv[])
{
    Compiler compiler = Compiler();

    if(string(argv[1]) == "create")
        compiler.print("Creating...");
    else
        compiler.run(argc, argv);

    return 0;
}
