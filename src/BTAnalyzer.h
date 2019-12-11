#ifndef BTAnalyzer_h
#define BTAnalyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
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
#include "ModuleHistos.h"
//Even == Top
//Odd == Bottom

class BTAnalyzer {
public :

   BTAnalyzer(TString inFile, unsigned int nMods, TString outFile);
   virtual ~BTAnalyzer();
   bool Init();
   void Loop();
   void offlineclusterizer(const std::vector<unsigned short>& hits, const unsigned int nCbc, const unsigned int nStripsPerCBC, std::vector<cluster>& clusVec );
   void stubSimulator(const std::vector<cluster>& seeding, const std::vector<cluster>& matching, 
                      std::vector<stub>& stubVec, const unsigned int clswCut = 3, const float window = 5.);

   void SaveHistos();
   void Reset();
   
private :
   void bookHistogram();
   void drawHistogram();   
   
   TH1I* topSensorHitProfile;
   TH1I* bottomSensorHitProfile;   
   
   TString inFile_;
   TString outFile_;
   TTreeReader* treeReader_;   //!pointer to the analyzed TTree or TChain
   unsigned int nmods_;
   TTreeReaderValue< std::vector< unsigned short > >* hitseven_[4];
   TTreeReaderValue< std::vector< unsigned short > >* hitsodd_[4];
   std::vector<cluster>  clustereven_[4];
   std::vector<cluster>  clusterodd_[4];
   std::vector<stub>     nstubs_[4];
   
   ModuleHistos*  mhists_[4];
   //
   TTreeReaderValue< std::vector< unsigned short > >* cbcerrors_;
   TTreeReaderValue< std::vector< unsigned short > >* cbcplladd_;
   TTreeReaderValue< std::vector< unsigned short > >* cbcl1_;

   bool raneventloop_;
};

#endif

