#include "ModuleHistos.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <sstream>
#include <cstdlib>

ModuleHistos::ModuleHistos(TString modname) {

    hposeven = new TH1I("hitseven_" + modname, "Top Sensor Hit Profile", 2032, -0.5, 2031.5);
    hposodd  = new TH1I("hitsodd_"  + modname, "Bottom Sensor Hit Profile", 2032, -0.5, 2031.5);

    clsposeven = new TH1F("clustereven_" + modname, "Top Sensor Cluster Profile", 2032, -0.5, 2031.5);
    clsposodd  = new TH1F("clusterodd_"  + modname, "Bottom Sensor Cluster Profile", 2032, -0.5, 2031.5);

    clswidtheven  = new TH1I("clusterWidtheven_" + modname, "Top Sensor cluster width", 20, -0.5, 19.5);
    clswidthodd  = new TH1I("clusterWidthodd_"  + modname, "Bottom Sensor cluster width", 20, -0.5, 19.5);

    stubpos  = new TH1F("stubpos_"  + modname, "Stub Profile", 2032, -0.5, 2031.5);
    nstubs  = new TH1I("nstubs_"  + modname, "Number of stubs", 10, -0.5, 9.5);
  }

ModuleHistos::~ModuleHistos() {
  std::cout << "Destroying module histo" << std::endl;
  if(hposeven)   delete hposeven;
  if(hposodd)   delete hposodd;
  if(clsposeven)   delete clsposeven;
  if(clsposodd)   delete clsposodd;
  if(clswidtheven)   delete clswidtheven;
  if(clswidthodd)   delete clswidthodd;
  if(nstubs)   delete nstubs;
  if(stubpos)   delete stubpos;
}
