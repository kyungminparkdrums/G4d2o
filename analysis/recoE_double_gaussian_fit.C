R__LOAD_LIBRARY(/home/kyungmip/G4d2o/G4d2o-build/simEvent/libsimEvent.so)
#include "simEvent.h"

void recoE_double_gaussian_fit() {

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


    TH1* dNorm = (TH1*)nPE_sourceE -> ProjectionX() -> Clone();
    dNorm -> SetXTitle("Source Energy (MeV)");
    dNorm -> SetYTitle("Gaussian normalization for numHits");
    dNorm -> SetName(Form("norm_%s", nPE_sourceE -> GetName()));
    dNorm -> SetTitle(Form("norm_%s", nPE_sourceE -> GetName()));

    TH1* dMean = (TH1*)nPE_sourceE -> ProjectionX() -> Clone();
    dMean -> SetXTitle("Source Energy (MeV)");
    dMean -> SetYTitle("Gaussian #mu for numHits");
    dMean -> SetName(Form("mean_%s", nPE_sourceE -> GetName()));
    dMean -> SetTitle(Form("#mu %s", nPE_sourceE -> GetName()));

    TH1* dSig = (TH1*)nPE_sourceE -> ProjectionX() -> Clone();
    dSig -> SetXTitle("Source Energy (MeV)");
    dSig -> SetYTitle("Gaussian #sigma for numHits");
    dSig -> SetName(Form("sigma_%s", nPE_sourceE -> GetName()));
    dSig -> SetTitle(Form("#sigma %s", nPE_sourceE -> GetName()));



    for (int iBin=1; iBin <= nPE_sourceE->GetNbinsX(); iBin++) {
        TH1D* h1 = (TH1D*)nPE_sourceE->ProjectionY(Form("nPE_sourceE_px_%d", iBin), iBin, iBin)->Clone();
        h1->Scale(1.0/h1->Integral());

        //if (iBin == 30 ) h1->Draw();

        TF1* f1 = new TF1("f", "gaus", 0, 2000);
        f1->SetParLimits(1, 0, 1000);
        f1->SetParLimits(2, 10, 400);

        h1->Fit(f1, "S0");    // First fit

        double muAttempt = std::max(f1->GetParameter(1), 16.*iBin);
        double sigAttempt = std::max(f1->GetParameter(2), 100.);

        h1->Fit(f1, "S0", "", muAttempt - sigAttempt, muAttempt + sigAttempt);     // Second fit within the range of parameters from first fit

        if (f1->GetParameter(1) < 1) f1->SetParameter(1, 0);

        dNorm -> SetBinContent(iBin, f1 -> GetParameter(0));
        dNorm -> SetBinError(iBin, f1 -> GetParError(0));
        dMean -> SetBinContent(iBin, f1 -> GetParameter(1));
        dMean -> SetBinError(iBin, f1 -> GetParError(1));
        dSig -> SetBinContent(iBin, f1 -> GetParameter(2));
        dSig -> SetBinError(iBin, f1 -> GetParError(2));

        //if (iBin == 30 ) f1->Draw("SAME");
    }

    TF1* peToMeV = new TF1("pePerMeV", "pol1", 0, 55);
    dMean -> Fit(peToMeV, "S0", "", 20, 40);

    dMean->Draw();
    peToMeV->Draw("SAME");

}
