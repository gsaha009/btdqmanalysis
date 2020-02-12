#include "ModuleHistos.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <sstream>
#include <cstdlib>

TrackHistos::TrackHistos() {
  bookHistos();
}

void TrackHistos::bookHistos() {
  refx_x = new TProfile("refx_x", "Reference plane x residual vs x",2000,-10,10);
  refx_y = new TProfile("refx_y", "Reference plane x residual vs y",2000,-10,10);
  refy_x = new TProfile("refy_x", "Reference plane y residual vs x",2000,-10,10);
  refy_y = new TProfile("refy_y", "Reference plane y residual vs y",2000,-10,10);

  nTk = new TH1I("nTk", "No. of tracks before cleaning at FeI4",  60, -0.5, 59.5);
  ncleanedTk = new TH1I("ncleanedTk", "No. of tracks after cleaning at FeI4", 60, -0.5, 59.5);

  resXfei4 = new TH1F("resXfei4", "Track x-residual at FeI4 plane", 2000, -10., 10.);
  resYfei4 = new TH1F("resYfei4", "Track y-residual at FeI4 plane", 2000, -10., 10.);
  matchedRefHitMap = new TH2D("matchedRefHitMap", "Matched Ref hits map", 2000, -10, 10, 3000, -15, 15);

}

void TrackHistos::writeHistostofile(TFile* fout, TString name) {
  fout->mkdir(name);//pass "tracks as name" 
  fout->cd(name); 
  nTk->Write();
  ncleanedTk->Write();
  resXfei4->Write();
  resYfei4->Write();
  refx_x->Write();
  refx_y->Write();
  refy_x->Write();
  refy_y->Write();
  matchedRefHitMap->Write();
}

void SensorHistos::bookHistos() {
  nhits_Fe0 = new TH1I("nHitsfe0_" + sensorId, "Hit profile FE0 for sensor " + sensorId, 60, -0.5, 59.5);
  nhits_Fe1 = new TH1I("nHitsfe1_" + sensorId, "Hit profile FE0 for sensor " + sensorId, 60, -0.5, 59.5); 
  hitpos_Fe0 = new TH1I("hitPositionfe0_" + sensorId, "Sensor hit profile FE0 " + sensorId, 1016, 1, 1017);
  hitpos_Fe1 = new TH1I("hitPositionfe1_" + sensorId, "Sensor hit profile FE1 " + sensorId, 1016, 1, 1017);

  ncls_Fe0 = new TH1I("nClusterfe0_" + sensorId, "Cluster profile FE0 for sensor " + sensorId, 60, -0.5, 59.5);
  ncls_Fe1 = new TH1I("nClusterfe1_" + sensorId, "Cluster profile FE1 for sensor " + sensorId, 60, -0.5, 59.5);
  clspos_Fe0 = new TH1F("clusterPosfe0_" + sensorId, "Cluster profile FE0 " + sensorId, 1016, 1, 1017);
  clspos_Fe1 = new TH1F("clusterPosfe1_" + sensorId, "Cluster profile FE1 " + sensorId, 1016, 1, 1017);

  clswidth_Fe0 = new TH1I("clusterWidthfe0_" + sensorId, "Cluster width FE0 " + sensorId, 20, -0.5, 19.5);
  clswidth_Fe1 = new TH1I("clusterWidthfe1_" + sensorId, "Cluster width FE1 " + sensorId, 20, -0.5, 19.5);
}

void SensorHistos::writeHistostofile(TFile* fout, TString modname) {
  fout->mkdir(modname + '/' + sensorId); 
  fout->cd(modname + '/' + sensorId);  

  nhits_Fe0->Write();
  nhits_Fe1->Write(); 

  hitpos_Fe0->Write();
  hitpos_Fe1->Write();

  ncls_Fe0->Write();
  ncls_Fe1->Write();

  clspos_Fe0->Write();
  clspos_Fe1->Write();

  clswidth_Fe0->Write();
  clswidth_Fe1->Write();  
}

//2 sensor per modules
void ModuleHistos::bookHistos() {
  bottomS.setSensorid(moduleId + "_bottom");
  bottomS.bookHistos();
  topS.setSensorid(moduleId + "_top");
  topS.bookHistos();

  ncbcstubsfe0 = new TH1I("nCbcstubsfe0_"   + moduleId, "CBC stubs for FE0 " + moduleId, 60, -0.5, 59.5);
  ncbcstubsfe1 = new TH1I("nCbcstubsfe1_"   + moduleId, "CBC stubs for FE1 " + moduleId, 60, -0.5, 59.5);

  cbcStubposfe0 = new TH1F("cbcStubposfe0_" + moduleId, "CBC stub profile FE0 " + moduleId, 1016, 1, 1017);
  cbcStubposfe1 = new TH1F("cbcStubposfe1_" + moduleId, "CBC stub profile FE1 " + moduleId, 1016, 1, 1017);

  nrecostubsfe0 = new TH1I("nRecostubsfe0_" + moduleId, "Reco stubs for FE0 " + moduleId, 60, -0.5, 59.5);
  nrecostubsfe1 = new TH1I("nRecostubsfe1_" + moduleId, "Reco stubs for FE1 " + moduleId, 60, -0.5, 59.5);

  recoStubposfe0 = new TH1F("recoStubposfe0_" + moduleId, "Reco stub profile FE0 " + moduleId, 1016, 1, 1017);
  recoStubposfe1 = new TH1F("recoStubposfe1_" + moduleId, "Reco stub profile FE1 " + moduleId, 1016, 1, 1017);

  clusterPoscorrfe0 = new TH2F("clsPoscorrfe0_" + moduleId, " Cluster position correlation Fe0 " + moduleId + "; Bottom sensor; Top sensor", 1016, 1, 1017, 1016, 1, 1017); 
  clusterPoscorrfe1 = new TH2F("clsPoscorrfe1_" + moduleId, " Cluster position correlation Fe1 " + moduleId + "; Bottom sensor; Top sensor", 1016, 1, 1017, 1016, 1, 1017); 

  clsvsStubPoscorrfe0 = new TH2F("clsvsStubPoscorrfe0_" + moduleId, " Fe0 " + moduleId + "; Cluster position; CBC Stub position", 1016, 1, 1017, 1016, 1, 1017); 
  clsvsStubPoscorrfe1 = new TH2F("clsvsStubPoscorrfe1_" + moduleId, " Fe1 " + moduleId + "; Cluster position; CBC Stub position", 1016, 1, 1017, 1016, 1, 1017); 

  rsvscsPoscorrfe0 = new TH2F("rsvscsPoscorrfe0_" + moduleId, " Fe0 " + moduleId + "; CBC Stub position; Reco Stub position", 1016, 1, 1017, 1016, 1, 1017); 
  rsvscsPoscorrfe1 = new TH2F("rsvscsPoscorrfe1_" + moduleId, " Fe1 " + moduleId + "; CBC Stub position; Reco Stub position", 1016, 1, 1017, 1016, 1, 1017);

  htdc = new TH1I("tdcphase_"   + moduleId, "; TDC;" , 10, -0.5, 9.5);
  htdc_matched = new TH1I("tdcphaseTkMatched_"   + moduleId, "TDC phase for Tk Matched event; TDC;" , 10, -0.5, 9.5);

  tkatDUTmap = new TH2D("tkatDUTmap", "Tk at DUT;x;y", 3000, -15, 15, 3000, -15, 15);
  tkVsDUTxcorr = new TH2D("tkVsDUTxcorr", ";track x  ;DUT x", 3000, -15, 15, 3000, -15, 15);
  tkVsDUTycorr = new TH2D("tkVsDUTycorr", ";track y  ;DUT y", 3000, -15, 15, 3000, -15, 15);

  resXdut = new TH1F("resXDUT", "Track x-residual at DUT", 3000, -15, 15);
  resYdut = new TH1F("resYDUT", "Track y-residual at DUT", 3000, -15, 15);

  tkxatDUT = new TH1F("tkxatDUT", "Track x at DUT", 3000, -15, 15);
  tkxatDUT_matched = new TH1F("tkxatDUT_matched", "Matched Track x at DUT", 3000, -15, 15);
  tkyatDUT = new TH1F("tkyatDUT", "Track y at DUT", 3000, -15, 15);
  tkyatDUT_matched = new TH1F("tkyatDUT_matched", "Matched Track y at DUT", 3000, -15, 15);
}
ModuleHistos::~ModuleHistos() {
  std::cout << "Destroying module histo" << std::endl;

}

void ModuleHistos::writeHistostofile(TFile* fout) {
  fout->mkdir(moduleId);
  fout->cd(moduleId);
  ncbcstubsfe0->Write();
  ncbcstubsfe1->Write();

  cbcStubposfe0->Write();
  cbcStubposfe1->Write();

  nrecostubsfe0->Write();
  nrecostubsfe1->Write();

  recoStubposfe0->Write();
  recoStubposfe1->Write();

  clusterPoscorrfe0->Write();
  clusterPoscorrfe1->Write();
  
  clsvsStubPoscorrfe0->Write();
  clsvsStubPoscorrfe1->Write();

  rsvscsPoscorrfe0->Write();
  rsvscsPoscorrfe1->Write();

  htdc->Write();
  htdc_matched->Write();

  tkatDUTmap->Write();
  tkVsDUTxcorr->Write();
  tkVsDUTycorr->Write();
  resXdut->Write();
  resYdut->Write();

  tkxatDUT->Write();
  tkxatDUT_matched->Write();
  tkyatDUT->Write();
  tkyatDUT_matched->Write();

  bottomS.writeHistostofile(fout, moduleId);
  topS.writeHistostofile(fout, moduleId);
}
