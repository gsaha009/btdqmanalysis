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
  //read DUT info
  treeReader_ = new TTreeReader("rawdata",fin);
  if(!treeReader_) {
    std::cout << "DUT tree not found in file\n";
    std::exit(EXIT_FAILURE);
  }

  //tkhists_
  tkhists_ = new TrackHistos();
  //loop over modules
  for(unsigned int i = 0; i < nmods_; i++ ) {
    std::cout << "Reading branch for module:" << i << std::endl;
    row_  = new TTreeReaderValue< std::vector< int > >(*treeReader_, "row");
    col_  = new TTreeReaderValue< std::vector< int > >(*treeReader_, "col");
    iden_ = new TTreeReaderValue< std::vector< int > >(*treeReader_, "iden");
    //create the container of histograms for this module
    mhists_[i] = new ModuleHistos(TString("Module_" + std::to_string(i)));
    mhists_[i]->bookHistos();
  }


  //read tracking info
  tktreeReader_ = new TTreeReader("tracks",fin);
  if(!tktreeReader_) {
    std::cout << "Track tree not found in file\n";
    std::exit(EXIT_FAILURE);
  }
  xPos_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "xPos");
  yPos_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "yPos");
  dxdz_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "dxdz");
  dydz_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "dydz");
  chi2_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "chi2");
  ndof_     = new TTreeReaderValue< std::vector< double > >(*tktreeReader_, "ndof");
  tkiden_   = new TTreeReaderValue< std::vector< int > >(*tktreeReader_, "iden");
  trackNum_ = new TTreeReaderValue< std::vector< int > >(*tktreeReader_, "trackNum");

  //read tracking info
  fptreeReader_ = new TTreeReader("fitpoints",fin);
  if(!fptreeReader_) {
    std::cout << "Track tree not found in file\n";
    std::exit(EXIT_FAILURE);
  }
  xPos_fp = new TTreeReaderValue< std::vector< double > >(*fptreeReader_, "xPos");
  yPos_fp = new TTreeReaderValue< std::vector< double > >(*fptreeReader_, "yPos");
  sensorId_fp = new TTreeReaderValue< std::vector< int > >(*fptreeReader_, "sensorId");
  clustersize_fp = new TTreeReaderValue< std::vector< int > >(*fptreeReader_, "clustersize");
  //read cbc stubinfo
  cbcstubtreeReader_ = new TTreeReader("stubs",fin);
  //ncbcStubs_ = new TTreeReaderValue<Int_t>(*cbcstubtreeReader_, "nStubs");
  cbcstubPos_    = new TTreeReaderValue< std::vector< int > >(*cbcstubtreeReader_, "stubPos");
  Fe_    = new TTreeReaderValue< std::vector< int > >(*cbcstubtreeReader_, "Fe");
  Cbc_    = new TTreeReaderValue< std::vector< int > >(*cbcstubtreeReader_, "Cbc");
  stubBend_ = new TTreeReaderValue< std::vector< int > >(*cbcstubtreeReader_, "stubBend");
  return true;   
    
}

//clear all the containers 
void BTAnalyzer::Reset() {
  //loop over modules
}
void BTAnalyzer::Loop()
{

  TTreeReaderValue<Int_t> ncbcStubs_(*cbcstubtreeReader_, "nStubs");  
  TTreeReaderValue<Int_t> tdc_(*treeReader_, "TDC");
  
  unsigned int noTk = 0;
  unsigned int tracksPerEvent = 0;
  unsigned int reftracksPerEvent = 0;
  unsigned int nRefTracks = 0;
  unsigned int nMatchedTracks = 0;
  
  while (treeReader_->Next() && tktreeReader_->Next() && fptreeReader_->Next() && cbcstubtreeReader_->Next()) {
    //clear all the containers
    Reset();
    int tdc = *(tdc_.Get());
    //Track information
    //match tracks at fei4 first. then use only selected tracks for analysis
    std::vector<Track>  cleanedTk;
    std::vector<int> ref_tracks;
    tkhists_->nTk->Fill((xPos_->Get())->size());
    noTk += (xPos_->Get())->size();
    //loop over tracks
    for (unsigned int it = 0; it < (tkiden_->Get())->size() ; it++)  {
      int identk = (tkiden_->Get())->at(it);   
      if(identk != 10)   continue;
      double xtk = (xPos_->Get())->at(it);
      double ytk = (yPos_->Get())->at(it);
      tracksPerEvent++;
      //loop over ref hits
      for (unsigned int iref = 0; iref < (sensorId_fp->Get())->size() ; iref++)  {

        int refId = (sensorId_fp->Get())->at(iref);
        if(refId != 10)    continue;//sensorId 10 is FeI4

        double xref = (xPos_fp->Get())->at(iref);
        double yref = (yPos_fp->Get())->at(iref);
        
        //residual fei4
        double xres = xref - xtk;
        double yres = yref - ytk;
        if (std::fabs(xres) < 1. && std::fabs(yres) < 1.) {
          tkhists_->resXfei4->Fill(xres);
          tkhists_->resYfei4->Fill(yres);
          tkhists_->refx_x->Fill(xtk, xres);
          tkhists_->refx_y->Fill(xtk, yres);
          tkhists_->refy_x->Fill(ytk, xres);
          tkhists_->refy_y->Fill(ytk, yres);
        }
        //residual cut at FeI4  
        if(std::fabs(xres) > 0.48 or std::fabs(yres) > 0.12 )   continue;
        tkhists_->matchedRefHitMap->Fill(xtk, ytk);
        ref_tracks.push_back((trackNum_->Get())->at(it));
        reftracksPerEvent++;
          /*cleanedTk.emplace_back(Track(xtk, ytk, 
                                    (dxdz_->Get())->at(it), 
                                    (dydz_->Get())->at(it), 
                                    (chi2_->Get())->at(it), 
                                    (ndof_->Get())->at(it),
                                    (tkiden_->Get())->at(it) ) );*/
        //if((tkiden_->Get())->at(it) == 30)
        }//end loop FeI4
      }//end loop over tracks
    tkhists_->ncleanedTk->Fill(cleanedTk.size());
    //ncleanTk+=cleanedTk.size();

    

    //loop over modules
    for(unsigned int i = 0; i < nmods_; i++ ) {
      ModuleHistos& mh = *(mhists_[i]);
      //create temp containers of hits per sensor and fe
      std::vector<int> hitsBotfe0;
      std::vector<int> hitsBotfe1;
      std::vector<int> hitsTopfe0;
      std::vector<int> hitsTopfe1;

      std::vector<cluster> clsBotfe0;
      std::vector<cluster> clsBotfe1;
      std::vector<cluster> clsTopfe0;
      std::vector<cluster> clsTopfe1;

      std::vector<stub> cbcstubfe0;
      std::vector<stub> cbcstubfe1;

      std::vector<stub> recostubfe0;
      std::vector<stub> recostubfe1;


      //TOP=30, BOTTOM=31
      //row 1 = FE0// row 0 = FE 1
      //for fe0, cbc strip = 1016 - strip
      for (unsigned int nh = 0; nh < (col_->Get())->size() ; nh++)  {
        const int sid =  (iden_->Get())->at(nh);
        const int r = (row_->Get())->at(nh);
        const int ch = (col_->Get())->at(nh);
        
        //TOP=30, BOTTOM=31
        //For bottom sensor, mask channel 794 for row==0
        if( sid == SENSORID::TOP) {
          if(r == 1) hitsTopfe0.push_back(ch);
          else if (r == 0) hitsTopfe1.push_back(ch);
        }
        else if( sid == SENSORID::BOTTOM) { 
          if(r == 1) hitsBotfe0.push_back(ch);
          else if (r == 0 and ch != 794) hitsBotfe1.push_back(ch);
        }
      }//finish loop over hits
      
      //Plot hit prop
      mh.bottomS.nhits_Fe0->Fill(hitsBotfe0.size());
      mh.bottomS.nhits_Fe1->Fill(hitsBotfe1.size());
      mh.topS.nhits_Fe0->Fill(hitsTopfe0.size());
      mh.topS.nhits_Fe1->Fill(hitsTopfe1.size());

      for (auto& h: hitsBotfe0) {
        mh.bottomS.hitpos_Fe0->Fill(h);
      }

      for (auto& h: hitsBotfe1) {
        mh.bottomS.hitpos_Fe1->Fill(h);
      }

      for (auto& h: hitsTopfe0) {
        mh.topS.hitpos_Fe0->Fill(h);
      }

      for (auto& h: hitsTopfe1) {
        mh.topS.hitpos_Fe1->Fill(h);
      }
      
      //create clusters
      offlineclusterizer(hitsBotfe0, 8, 127, clsBotfe0);
      offlineclusterizer(hitsBotfe1, 8, 127, clsBotfe1);
      offlineclusterizer(hitsTopfe0, 8, 127, clsTopfe0);
      offlineclusterizer(hitsTopfe1, 8, 127, clsTopfe1);
    
      //Plot clus prop
      mh.bottomS.ncls_Fe0->Fill(clsBotfe0.size());
      mh.bottomS.ncls_Fe1->Fill(clsBotfe1.size());
      mh.topS.ncls_Fe0->Fill(clsTopfe0.size());
      mh.topS.ncls_Fe1->Fill(clsTopfe1.size());
      for (auto& h: clsBotfe0) {
        mh.bottomS.clspos_Fe0->Fill(h.center());
        mh.bottomS.clswidth_Fe0->Fill(h.size());
      }

      for (auto& h: clsBotfe1) {
        mh.bottomS.clspos_Fe1->Fill(h.center());
        mh.bottomS.clswidth_Fe1->Fill(h.size());
      }

      for (auto& h: clsTopfe0) {
        mh.topS.clspos_Fe0->Fill(h.center());
        mh.topS.clswidth_Fe0->Fill(h.size());
      }

      for (auto& h: clsTopfe1) {
        mh.topS.clspos_Fe1->Fill(h.center());
        mh.topS.clswidth_Fe1->Fill(h.size());
      }
      
      //Correlation histos
      for(auto& h0: clsBotfe0) {
        for(auto& h1: clsTopfe0)
          mh.clusterPoscorrfe0->Fill(h0.center(), h1.center());
      }
      
      for(auto& h0: clsBotfe1) {
        for(auto& h1: clsTopfe1)
          mh.clusterPoscorrfe1->Fill(h0.center(), h1.center());
      }

      //CBC Stub information
      for(unsigned int is = 0; is <  (cbcstubPos_->Get())->size(); is++) {
        int fe   = (Fe_->Get())->at(is);
        int cbc  = (Cbc_->Get())->at(is);
        int pos  = (cbcstubPos_->Get())->at(is);
        int bend = (stubBend_->Get())->at(is);
        float strip = cbc*127 + pos/2.;
        if (fe == 0) {
          strip = 1016 - strip;
          cbcstubfe0.emplace_back(stub(strip, fe, bend));
        } else if (fe == 1) cbcstubfe1.emplace_back(stub(strip, fe, bend));
      }//cbc stub loop
      //plot cbc stub prop
      mh.ncbcstubsfe0->Fill(cbcstubfe0.size());
      for(auto& s : cbcstubfe0) {
        mh.cbcStubposfe0->Fill(s.center());
        for (auto& h: clsBotfe0) {
          mh.clsvsStubPoscorrfe0->Fill(h.center(), s.center());
        }
      }
      mh.ncbcstubsfe1->Fill(cbcstubfe1.size());
      for(auto& s : cbcstubfe1) {
        mh.cbcStubposfe1->Fill(s.center());
        for (auto& h: clsBotfe1) {
          mh.clsvsStubPoscorrfe1->Fill(h.center(), s.center());
        }
      }

      //now do offline stub reco 
      //seeding, matching, stubVec
      stubSimulator(clsBotfe0, clsTopfe0, recostubfe0);//bottom is seeding
      stubSimulator(clsBotfe1, clsTopfe1, recostubfe1);//bottom is seeding

      //plot reco stub prop
      mh.nrecostubsfe0->Fill(recostubfe0.size());
      for(auto& rs : recostubfe0) {
        mh.recoStubposfe0->Fill(rs.center());
        for (auto& cs: cbcstubfe0) {
          mh.rsvscsPoscorrfe0->Fill(cs.center(), rs.center());
        }
      }
      mh.nrecostubsfe1->Fill(recostubfe1.size());
      for(auto& rs : recostubfe1) {
        mh.recoStubposfe1->Fill(rs.center());
        for (auto& cs: cbcstubfe1) {
          mh.rsvscsPoscorrfe1->Fill( cs.center(), rs.center() );
        }
      }

      //now to Tk to DUT matching
      // If there is ref hit, take the last one
      // if (ref_tracks>-1)
      bool tdcok = tdc == 2 or tdc == 3 or tdc == 4;
      if(!tdcok)   continue;

      for (unsigned int irt =0; irt < ref_tracks.size(); irt++) {
	int ref_track = ref_tracks[irt];
	int trackID = -1;
	for (unsigned int it=0; it < (tkiden_->Get())->size(); it++) {
	  if ((trackNum_->Get())->at(it) == ref_track && (tkiden_->Get())->at(it) == 30) {
	    trackID=it;
	    nRefTracks++;
	    break;
	  }
	}
        //Track at DUT plane
        double txpos = (xPos_->Get())->at(trackID);
        double typos = (yPos_->Get())->at(trackID);

        mh.tkatDUTmap->Fill(txpos, typos);
        mh.tkxatDUT->Fill(txpos);
        mh.tkyatDUT->Fill(typos);
        //Fill denominator tdc histo
        mh.htdc->Fill(tdc);
        bool isMatched = false;
        
        bool tkMcount[10] = {0};
        //now loop over sensor hits with sensor ID = 30, i.e. DUT
        for (unsigned int iref = 0; iref < (sensorId_fp->Get())->size() ; iref++)  {
          int refId = (sensorId_fp->Get())->at(iref);
          if(refId != 30)    continue;//sensorId 30 is DUT
          double xdut = (xPos_fp->Get())->at(iref);
          double ydut = (yPos_fp->Get())->at(iref);
          int clsize = (clustersize_fp->Get())->at(iref);
          
          //residual DUT
          double xres = xdut - txpos;
          double yres = ydut - txpos;

          mh.tkVsDUTxcorr->Fill(txpos, xdut);
          mh.tkVsDUTycorr->Fill(typos, ydut);
          mh.resXdut->Fill(xres);
          mh.resYdut->Fill(yres);
          for(unsigned int ires = 0; ires < 10; ires++) {
            float cut = (ires+1) * 0.05;
            if ( std::fabs(xres)< cut) 
              tkMcount[ires] = true;
            
          }
          //This cut has to be varied to plot efficiency as a function of cut
          if ( std::fabs(xres)<0.200 /*&& fabs(dy)<2.5*/) {
	    nMatchedTracks++;
            isMatched = true;           
          }
        }//end loop over sensor hits
        if(isMatched)     {
          mh.htdc_matched->Fill(tdc);
          mh.tkxatDUT_matched->Fill(txpos);
          mh.tkyatDUT_matched->Fill(typos);
        }
        for(unsigned int ires = 0; ires < 10; ires++) 
          if(tkMcount[ires]) mh.tkmatchCounter->Fill((ires+1)*50);
         
      }
      //float s = 1./float(nRefTracks);
      //mh.tkmatchCounter->Scale(s);
      //for(unsigned int ires = 1; ires <= 10; ires++)
      //  mh.tkmatchCounter->SetBinContent(ires, mh.tkmatchCounter->GetBinContent(ires)/float(nRefTracks));  


    }//for loop over modules

  }//event loop while end
  raneventloop_ = true;
  std::cout << "Total tracks =" << tracksPerEvent << std::endl; 
  std::cout << "nRefTracks = " << nRefTracks << std::endl;
  std::cout << "nMatchedTracks = " << nMatchedTracks << std::endl;
  std::cout << "Efficiency: " << (float)nMatchedTracks/nRefTracks*100.  << "%" << std::endl;
}//end event loop 

//Function to create clusters from hits
void BTAnalyzer::offlineclusterizer(const std::vector<int>& hits, const unsigned int nCbc, const unsigned int nStripsPerCBC, std::vector<cluster>& clusVec ){
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
  tkhists_->writeHistostofile(fout, "tracks");
  //loop over modules
  for(unsigned int i = 0; i < nmods_; i++ ) {  
    ModuleHistos& mh = *(mhists_[i]);
    fout->cd(); 
    mh.writeHistostofile(fout); 
    
  }
  fout->Save();
  fout->Close();
}
