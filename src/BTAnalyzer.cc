#include "BTAnalyzer.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <sstream>
#include <cstdlib>

BTAnalyzer::BTAnalyzer(TString inFile, unsigned int nMods, TString outFile) : treeReader_(0) {
  inFile_ = inFile;
  nmods_ = nMods;
  outFile_ = outFile;
  raneventloop_ = false;
  if(!Init()) {
    std::cout << "Initialization error\n";
    std::exit(EXIT_FAILURE);
  } 
}

BTAnalyzer::~BTAnalyzer() {
  
  std::cout << "Destroying pointers!!" << std::endl;
  if (treeReader_) delete treeReader_;
}


bool BTAnalyzer::Init() {
  TFile* fin = TFile::Open(inFile_);
  if(!fin)   {
    std::cout << "File doesn't exist\n";
    std::exit(EXIT_FAILURE);
  }
  
  treeReader_ = new TTreeReader("dqm",fin);
  if(!treeReader_) {
    std::cout << "Tree not found in file\n";
    std::exit(EXIT_FAILURE);
  }
  //loop over modules
  for(unsigned int i = 0; i < nmods_; i++ ) {
    std::cout << "Reading branch for module:" << i << std::endl;
    //even channels
    std::string hitkey = "hitEven_Mod_" + std::to_string(i);
    hitseven_[i] = new TTreeReaderValue< std::vector< unsigned short > >(*treeReader_, hitkey.c_str());
    //odd channels
    hitkey = "hitOdd_Mod_" + std::to_string(i);
    hitsodd_[i] = new TTreeReaderValue< std::vector< unsigned short > >(*treeReader_, hitkey.c_str());

    //Similarly read the cbc erros etc.

    //create the container of histograms for this module
    mhists_[i] = new ModuleHistos(TString("Module" + std::to_string(i)));
  }

  return true;   
    
}

//clear all the containers 
void BTAnalyzer::Reset() {
  //loop over modules
  for(unsigned int i = 0; i < nmods_; i++ ) {
    clustereven_[i].clear();
    clusterodd_[i].clear();
    nstubs_[i].clear();
  }

}

void BTAnalyzer::Loop()
{

  while (treeReader_->Next()) {
    //clear all the containers
    Reset();

    //loop over modules
    for(unsigned int i = 0; i < nmods_; i++ ) {
      ModuleHistos& mh = *(mhists_[i]);
      //Loop over even channels
      for (float h: *(hitseven_[i]->Get())) {
        mh.hposeven->Fill(h);
      }
      //Loop over odd channels
      for (float h: *(hitsodd_[i]->Get())) {
        mh.hposodd->Fill(h);
      }

      //do clustering

      offlineclusterizer(*(hitseven_[i]->Get()), 16, 127, clustereven_[i]);
      offlineclusterizer(*(hitsodd_[i]->Get()), 16, 127, clusterodd_[i]);

      //make plots for clusters
      for(auto& c : clustereven_[i]) {
        mh.clsposeven->Fill(c.center());
        mh.clswidtheven->Fill(c.size());
      }
      for(auto& c : clusterodd_[i]) {
        mh.clsposodd->Fill(c.center());
        mh.clswidthodd->Fill(c.size());
      }

      //for stubs
      //Even == Top Odd == Bottom; Bottom = Seeding//F
      stubSimulator(clusterodd_[i], clustereven_[i], nstubs_[i]);
      mh.nstubs->Fill(nstubs_[i].size());
      for(auto& stub : nstubs_[i]) {
        mh.stubpos->Fill(stub.center());
      }

    }
  }
  raneventloop_ = true;
}//end event loop 

//Function to create clusters from hits
void BTAnalyzer::offlineclusterizer(const std::vector<unsigned short>& hits, const unsigned int nCbc, const unsigned int nStripsPerCBC, std::vector<cluster>& clusVec ){
  if (hits.empty())  return; 
  unsigned int fStrip = hits.at(0);//get strip of first hit
  unsigned int col    = hits.at(0)/16 < 8 ?  0 : 1;//0-1015 is col 0 and 1016-2031 is col 1
  //what can be done? add a chip variable in cluster object and fill the cluster info here
  //should be similar to what is done in DQM/ This info can then be propagated to stubs as well.
  unsigned int size=1;
  //unsigned int edge = 8*nStripsPerCBC;
  for (unsigned int i = 1; i < hits.size(); i++){
    unsigned int icol    = hits.at(i)/16 < 8 ?  0 : 1;
    //form vectors of with hits from the same column
    if (hits.at(i) == fStrip + size && icol == col){
      size++;
    }
    else{
      cluster clust(fStrip, col, size);
      clusVec.push_back(clust);
      //reset the intial parameters
      size=1;
      fStrip = hits.at(i);//get strip of first hit
      col    = icol;
    }  
  }       
  cluster clust(fStrip, col, size);
  clusVec.push_back(clust);
}

//function to create stubs from clusters
void BTAnalyzer::stubSimulator(const std::vector<cluster>& seeding, const std::vector<cluster>& matching, std::vector<stub>& stubVec, 
                               const unsigned int clswCut, const float window ){
  for(auto& sCls : seeding) {
    if(sCls.size() > clswCut)       continue;//cut cluster size
    for(auto& mCls :matching) {
      if(mCls.size() > clswCut)     continue;//cut cluster size
      //this is a FP operation. Wondering if it is better to save all FP positions as uint32(multipying by 2)
      //So the idea would be: a cluster at pos = 100.5 will be represented as 201
      //the window is an integer, so converting uint32 can be a better option//someone should check.
      if(std::abs(sCls.center() - mCls.center()) <= window) {
        stubVec.emplace_back( stub(sCls.firstStrip(), sCls.column(), sCls.center() - mCls.center()) );
      }
    }
  }
}

//Save histograms in outpur root file. One folder per module.
//have to think how to organize inter module correlation
void BTAnalyzer::SaveHistos() {
  if(!raneventloop_) std::cout << "You are trying to save histograms without running the event loop!" << std::endl; 
  TFile* fout = TFile::Open(outFile_, "recreate");
  fout->cd();
  //loop over modules
  for(unsigned int i = 0; i < nmods_; i++ ) {  
    TString modkey("Module_" + std::to_string(i));
    fout->mkdir(modkey);
    fout->cd(modkey);
    ModuleHistos& mh = *(mhists_[i]);
    mh.hposeven->Write();
    mh.hposodd->Write();
    mh.clsposeven->Write();
    mh.clsposodd->Write();
    mh.clswidtheven->Write();
    mh.clswidthodd->Write();
    mh.nstubs->Write(); 
    mh.stubpos->Write();
    fout->cd();  
  }
  fout->Save();
  fout->Close();
}
