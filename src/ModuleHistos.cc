#include "ModuleHistos.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <sstream>
#include <cstdlib>

ModuleHistos::ModuleHistos(TString modname) {

    nhitsodd = new TH1I("nHitsEven_" + modname, "Top Sensor Hit Profile", 60, -0.5, 59.5);
    nhitseven  = new TH1I("nHitsOdd_"  + modname, "Bottom Sensor Hit Profile", 60, -0.5, 59.5);

    hitposodd = new TH1I("hitPositionEven_" + modname, "Top Sensor Hit Profile", 2032, -0.5, 2031.5);
    hitposeven  = new TH1I("hitPositionOdd_"  + modname, "Bottom Sensor Hit Profile", 2032, -0.5, 2031.5);

    nclsodd = new TH1F("nClusterEven_" + modname, "Number of Clusters (Top Sensor)", 60, -0.5, 59.5);
    nclseven  = new TH1F("nClusterOdd_"  + modname, "Number of Clusters (Bottom Sensor)", 60, -0.5, 59.5);
    nclstot  = new TH1F("nClusterTotal_"  + modname, "Total Number of Clusters (Top+Bottom Sensor)", 100, -0.5,99.5);

    clsposodd = new TH1F("clusterPosEven_" + modname, "Top Sensor Cluster Profile", 2032, -0.5, 2031.5);
    clsposeven  = new TH1F("clusterPosOdd_"  + modname, "Bottom Sensor Cluster Profile", 2032, -0.5, 2031.5);

    clswidthodd  = new TH1I("clusterWidthEven_" + modname, "Top Sensor cluster width", 20, -0.5, 19.5);
    clswidtheven  = new TH1I("clusterWidthOdd_"  + modname, "Bottom Sensor cluster width", 20, -0.5, 19.5);

    stubpos  = new TH1F("stubPosison_"  + modname, "Stub Profile", 2032, -0.5, 2031.5);
    nstubs  = new TH1I("nStubs_"  + modname, "Number of stubs", 10, -0.5, 9.5);

    nclstdcevenprof = new TH2F("nClusterEvenVsTDC_" + modname, " Number of Cluster Vs TDC (Bottom Sensor)", 8, -0.5, 7.5, 60, -0.5, 59.5);
    nclstdcoddprof = new TH2F("nClusterOddVsTDC_" + modname, " Number of Cluster Vs TDC (Top Sensor)", 8, -0.5, 7.5, 60, -0.5, 59.5);

    nhitstdcevenprof  = new TH2F("nHitsEvenVsTDC_" + modname, " Number of Cluster Vs TDC (Bottom Sensor)", 8, -0.5, 7.5, 60, -0.5, 59.5);
    nhitstdcoddprof = new TH2F("nHitsOddVsTDC_" + modname, " Number of Hits Vs TDC (Top Sensor)", 8, -0.5, 7.5, 60, -0.5, 59.5);

    clwidthtdcevenprof = new TH2F("clusterWidthEvenVsTDC_" + modname, " Cluster Width Vs TDC (Bottom Sensor)", 8, -0.5, 7.5, 20, -0.5, 19.5);
    clwidthtdcoddprof = new TH2F("clusterWidthOddVsTDC_" + modname, " Cluster Width Vs TDC (Top Sensor)", 8, -0.5, 7.5, 20, -0.5, 19.5);

  }

ModuleHistos::~ModuleHistos() {
  std::cout << "Destroying module histo" << std::endl;
  if(nhitseven)   delete nhitseven;
  if(nhitsodd)   delete nhitsodd;
  if(hitposeven)   delete hitposeven;
  if(hitposodd)   delete hitposodd;
  if(nclseven)  delete nclseven;
  if(nclsodd)   delete nclsodd;
  if(nclstot)   delete nclstot;
  if(clsposeven)   delete clsposeven;
  if(clsposodd)   delete clsposodd;
  if(clswidtheven)   delete clswidtheven;
  if(clswidthodd)   delete clswidthodd;
  if(nstubs)   delete nstubs;
  if(stubpos)   delete stubpos;
  if (nclstdcevenprof) delete nclstdcevenprof;
  if (nclstdcoddprof) delete nclstdcoddprof;
  if (nhitstdcevenprof) delete nclstdcevenprof;
  if (nhitstdcoddprof) delete nclstdcoddprof;
  if (clwidthtdcevenprof) delete clwidthtdcevenprof;
  if (clwidthtdcoddprof) delete clwidthtdcoddprof;
}
