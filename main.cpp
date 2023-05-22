#include <iostream>
#include <cstring>
#include <string>

#include "ProblemInstance.h"

int main(int argc, char** argv){

    std::string mapFName;
 
    for (int i = 1; i < argc; i++){
        std::cout << argv[i] << "\n";

        if (strcmp(argv[i], "-m") == 0) {
            mapFName = argv[i+1];
        }

        i++;

    }

    std::cout << "Map File Name is " << mapFName << std::endl;
        


 
    return 0;
}