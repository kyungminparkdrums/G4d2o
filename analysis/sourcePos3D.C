R__LOAD_LIBRARY(/home/kyungmip/G4d2o/G4d2o-build/simEvent/libsimEvent.so)
#include "simEvent.h"

void sourcePos3D(){

    // Import file and get TTree
    TString* fname = new TString("$G4D2O/data/electron_primaries/Sim_D2ODetector001.root");

    TChain* Sim_Tree = new TChain("Sim_Tree", "Sim_Tree");
    Sim_Tree->Add(fname->Data());

    simEvent *ev = new simEvent();
    Sim_Tree->SetBranchAddress("eventData", &ev);

    // Output file and histograms
    TFile *outFile = new TFile("sourcePos3D.root", "RECREATE");

    TH3D* D2O_source = new TH3D("D2O_source_particle", "D2O_source_particle", 100, -500., 500., 100, -500., 500., 200, -1000, 1000);
    TH3D* H2O_source = new TH3D("H2O_source_particle", "H2O_source_particle", 100, -500., 500., 100, -500., 500., 200, -1000, 1000);

    // Event loop
    for (int iEvent=0; iEvent < Sim_Tree->GetEntries(); iEvent++) {
        Sim_Tree->GetEntry(iEvent);

        TVector3 posInitial = ev->position0;           // position of source
	int vol0 = ev->vol0;

        double nPE = ev->numHits;   // number of PE's seen by the detector, dimensionless
        if (nPE > 0) {
            if (vol0 == 1) D2O_source->Fill(posInitial.x(), posInitial.y(), posInitial.z()); 
       }
    }

    // Some drawing options
    D2O_source->SetOption("BOX2");
    H2O_source->SetOption("BOX2");

    D2O_source->SetStats(0);
    H2O_source->SetStats(0);

    D2O_source->Write();
    H2O_source->Write();

    // Project the 3D plot to 2D xy plane
    TH2D* D2O_xy = (TH2D*)D2O_source->Project3D("xy");
    TH2D* H2O_xy = (TH2D*)H2O_source->Project3D("xy");

    D2O_xy->SetOption("colz");
    H2O_xy->SetOption("colz");

    D2O_xy->SetStats(0);
    H2O_xy->SetStats(0);

    D2O_xy->Write();
    H2O_xy->Write();

    outFile->Close();


}
