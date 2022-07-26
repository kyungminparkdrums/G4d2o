R__LOAD_LIBRARY(/home/kyungmip/G4d2o/G4d2o-build/simEvent/libsimEvent.so)
#include "simEvent.h"

void recoE_fit2D() {

    TString* fname = new TString("$G4D2O/data/electron_primaries/Sim_D2ODetector001.root");

    TChain* Sim_Tree = new TChain("Sim_Tree", "Sim_Tree");
    Sim_Tree->Add(fname->Data());

    simEvent *ev = new simEvent();
    Sim_Tree->SetBranchAddress("eventData", &ev);

    // 2D histogram of nPE vs. Esrc
    TH2D* nPE_sourceE = new TH2D("nPE_vs_sourceE", "nPE_vs_sourceE", 50, 0, 50, 150, 0, 1500);

    for (int iEvent=0; iEvent < Sim_Tree->GetEntries(); iEvent++) {
        Sim_Tree->GetEntry(iEvent);

        int nPE = ev->numHits;
        double sourceE = ev->sourceParticleEnergy;

        nPE_sourceE->Fill(sourceE, nPE);
    }

    nPE_sourceE->GetXaxis()->SetTitle("Source Particle Energy (MeV)");
    nPE_sourceE->GetYaxis()->SetTitle("Number of PE's");
    nPE_sourceE->SetTitle("nPE vs. Esrc");

    nPE_sourceE->SetStats(0);

    nPE_sourceE->Draw("colz");

    //nPE_sourceE->ProjectionX()->Draw();    // Source Particle Energy
    //nPE_sourceE->ProjectionY()->Draw();    // Number of PE's

    TF1* gaus = new TF1("f1", "[0]+[1]*x", 0, 1);
    nPE_sourceE->Fit(gaus);
    gaus->Draw("same");

}
