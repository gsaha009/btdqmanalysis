#ifndef ModuleHistos_h
#define ModuleHistos_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TFile.h>
#include <TString.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Header file for the classes stored in the TTree if any.
#include<iostream>
#include<map>
#include "Cluster.h"
#include "Stub.h"

//Even == Top
//Odd == Bottom
class ModuleHistos {
public:
  ModuleHistos(TString modname);
  
  virtual ~ModuleHistos();

  TH1I* nhitseven;
  TH1I* nhitsodd;
  TH1I* hitposeven;
  TH1I* hitposodd;
  TH1F* clsposeven;
  TH1F* nclsodd;
  TH1F* nclseven;
  TH1F* nclstot;  
  TH1F* clsposodd;
  TH1I* clswidtheven;
  TH1I* clswidthodd;
  TH1I* nstubs;
  TH1F* stubpos;
  TH2F* nclstdcevenprof;
  TH2F* nclstdcoddprof;
  TH2F* nhitstdcevenprof;
  TH2F* nhitstdcoddprof;
  TH2F* clwidthtdcevenprof;
  TH2F* clwidthtdcoddprof;
};
#endif

