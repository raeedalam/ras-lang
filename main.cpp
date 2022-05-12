#include <iostream>
#include "compiler.h"
#include <fstream>

int main(int argc, char *argv[])
{
    Compiler compiler = Compiler();

    if(argc > 1 && string(argv[1]) == "create") {
        compiler.print("Creating...");
        ofstream MyFile(string(argv[2]) + ".ras");
        compiler.print("Finished");
    }
    else if(argc > 1 && string(argv[1]) == "pkg") {
        if(string(argv[2]) == "install") {
            system((string("wget ")+string(argv[3])).c_str());
            compiler.print("Installed " + string(argv[3]) + " to project.");
        };
    }
    else {
        compiler.run(argc, argv);
    }

    return 0;
}
