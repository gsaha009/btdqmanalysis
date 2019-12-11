#include "BTAnalyzer.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cout << "usage: ./btanalysis <inFilename> <nmodules> <outFilename>\n";
    std::exit(EXIT_FAILURE);  
  }

  std::string iFile = argv[1];
  unsigned int nmodules = std::stoi(argv[2]);
  std::string oFile = argv[3];


  BTAnalyzer *analyzer = new  BTAnalyzer(iFile, nmodules, oFile);
  std::cout << "Executing event loop!" << std::endl;
  analyzer->Loop();
  std::cout << "Saving Histos!" << std::endl;
  analyzer->SaveHistos();

  if(analyzer)   delete analyzer;

  return 0;
}





