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
#include <TProfile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Header file for the classes stored in the TTree if any.
#include<iostream>
#include<map>
#include "Cluster.h"
#include "Stub.h"

//this class will book and fill all Track related properties
//not DUT + Track matching properties. Those will be taken care 
//by inidividual ModuleHistos
class TrackHistos {
  public:
    TrackHistos();
    void bookHistos();
    void writeHistostofile(TFile* fout, TString name);

    TProfile* refx_x;
    TProfile* refx_y;
    TProfile* refy_x; 
    TProfile* refy_y; 
    TH1F* resXfei4;
    TH1F* resYfei4;
    TH1I* nTk;
    TH1I* ncleanedTk;
    TH2D* matchedRefHitMap;

};

//Even == Top
//Odd == Bottom
class SensorHistos {
  public:
    TString sensorId;
    SensorHistos() {}
    SensorHistos(TString sId) {
      sensorId = sId;
    }

    void setSensorid(TString sId) {
      sensorId = sId;
    }
    void bookHistos();
    void writeHistostofile(TFile* fout, TString modname);

    TH1I* nhits_Fe0;
    TH1I* nhits_Fe1; 

    TH1I* hitpos_Fe0;
    TH1I* hitpos_Fe1;

    TH1I* ncls_Fe0;
    TH1I* ncls_Fe1;

    TH1F* clspos_Fe0;
    TH1F* clspos_Fe1;
 
    TH1I* clswidth_Fe0;
    TH1I* clswidth_Fe1;   
};

class ModuleHistos {
public:

  ModuleHistos() {}

  ModuleHistos(TString modname) {
    moduleId = modname;
  }
  void setmoduleid(TString modname) {
    moduleId = modname;
  }

  void writeHistostofile(TFile* fout);
  void bookHistos();
  virtual ~ModuleHistos();
  TString moduleId;

  TH1I* ncbcstubsfe0;
  TH1I* ncbcstubsfe1;

  TH1F* cbcStubposfe0;
  TH1F* cbcStubposfe1;

  TH1I* nrecostubsfe0;
  TH1I* nrecostubsfe1;

  TH1F* recoStubposfe0;
  TH1F* recoStubposfe1;

  TH2F* clusterPoscorrfe0;
  TH2F* clusterPoscorrfe1;
  
  TH2F* clsvsStubPoscorrfe0;
  TH2F* clsvsStubPoscorrfe1;

  TH2F* rsvscsPoscorrfe0;
  TH2F* rsvscsPoscorrfe1;

  TH1I* htdc;
  TH1I* htdc_matched;

  TH2D* tkatDUTmap;
  TH2D* tkVsDUTxcorr;
  TH2D* tkVsDUTycorr;

  TH1F* resXdut;
  TH1F* resYdut;

  TH1F* tkxatDUT;
  TH1F* tkxatDUT_matched;
  TH1F* tkyatDUT;
  TH1F* tkyatDUT_matched;


  SensorHistos bottomS;
  SensorHistos topS;

};
#endif

