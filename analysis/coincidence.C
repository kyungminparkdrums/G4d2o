R__LOAD_LIBRARY(/home/kyungmip/G4d2o/G4d2o-build/simEvent/libsimEvent.so)
#include "simEvent.h"

void coincidence(){

    // Import file and get TTree
    TString* fname = new TString("$G4D2O/data/michel_primaries/Sim_D2ODetector001.root");

    TChain* Sim_Tree = new TChain("Sim_Tree", "Sim_Tree");
    Sim_Tree->Add(fname->Data());

    simEvent *ev = new simEvent();
    Sim_Tree->SetBranchAddress("eventData", &ev);

    // Output histograms and file
    TFile *outFile = new TFile("coincidence.root", "RECREATE");

    TH1D* D2O_coinc = new TH1D("D2O", "D2O", 13, 0, 13);
    TH1D* H2O_coinc = new TH1D("H2O", "H2O", 13, 0, 13);
    TH1D* All_coinc = new TH1D("All", "All", 13, 0, 13);
    TH1D* D2O_frac_coinc = new TH1D("D2O_Fraction", "D2O_Fraction", 13, 0, 13);
    TH1D* D2O_sig_coinc = new TH1D("D2O_Significance", "D2O_Significance", 13, 0, 13);

    // Event loop
    for (int iEvent=0; iEvent < Sim_Tree->GetEntries(); iEvent++) {
        Sim_Tree->GetEntry(iEvent);

	int vol0 = ev->vol0;
	double nPE = ev->numHits;   // number of PE's seen by the detector, dimensionless

        std::vector<int> hits(12, 0);  // initialize vector with size 12, each element in the vector will have info of numHits in the PMT number, initialize the vector with 0's for now

	// Calculate coincidence
	int coinc = 0;
	if (nPE > 0) {
            for (int iHit = 0; iHit < nPE; iHit++) {
                const simHit *hit = ev->GetHit(iHit);
                int pmtNum = hit->pmtNum;

		hits[pmtNum] += 1;
            }
	    coinc = hits.size() - count(hits.begin(), hits.end(), 0);   // coincidence
        }
      
       // Fill the coincidence histogram for D2O and H2O	
       if (vol0 == 1) { 
           D2O_coinc->Fill(coinc);
           All_coinc->Fill(coinc);
       }
       else if (vol0 == 2) { 
           H2O_coinc->Fill(coinc);
           All_coinc->Fill(coinc);
       }
    }

    // Set axes
    D2O_coinc->GetYaxis()->SetRangeUser(0., D2O_coinc->GetMaximum()*1.3);
    H2O_coinc->GetYaxis()->SetRangeUser(0., H2O_coinc->GetMaximum()*1.3);
    All_coinc->GetYaxis()->SetRangeUser(0., All_coinc->GetMaximum()*1.3);

    D2O_coinc->GetYaxis()->SetTitle("Number of events / bin");
    H2O_coinc->GetYaxis()->SetTitle("Number of events / bin");
    All_coinc->GetYaxis()->SetTitle("Number of events / bin");

    D2O_coinc->GetXaxis()->SetTitle("Coincidence");
    H2O_coinc->GetXaxis()->SetTitle("Coincidence");
    All_coinc->GetXaxis()->SetTitle("Coincidence");
  
    // Write to file
    D2O_coinc->Write();
    H2O_coinc->Write();
    All_coinc->Write();


    // Calculate fraction of D2O events and significance of D2O events
    for (int iBin=1; iBin < D2O_coinc->GetNbinsX()+1; iBin++) {
        int d2o_entries = D2O_coinc->GetBinContent(iBin);
        int h2o_entries = H2O_coinc->GetBinContent(iBin);
        double all_entries = All_coinc->GetBinContent(iBin);

	double d2o_frac = d2o_entries / all_entries;
	if (d2o_frac != d2o_frac) d2o_frac = 0.;
    	D2O_frac_coinc->SetBinContent(iBin, d2o_frac);

        double d2o_sig = d2o_entries / sqrt(h2o_entries);
	if (d2o_sig != d2o_sig) d2o_sig = 0.;
	D2O_sig_coinc->SetBinContent(iBin, d2o_sig);
    }

    // Do not show stat box
    D2O_frac_coinc->SetStats(0);
    D2O_sig_coinc->SetStats(0);

    // Axes setting
    D2O_frac_coinc->GetYaxis()->SetTitle("n(D2O events) / n(D2O & H2O events)");
    D2O_sig_coinc->GetYaxis()->SetTitle("n(D2O events) / sqrt(n(H2O events))");

    D2O_frac_coinc->GetXaxis()->SetTitle("Coincidence");
    D2O_sig_coinc->GetXaxis()->SetTitle("Coincidence");
 
    // Write to file
    D2O_frac_coinc->Write();
    D2O_sig_coinc->Write();

    outFile->Close();

}
