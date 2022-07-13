R__LOAD_LIBRARY(/home/kyungmip/G4d2o/G4d2o-build/simEvent/libsimEvent.so)
#include "simEvent.h"

void eventTime(){

    // Import file and get TTree
    TString* fname = new TString("$G4D2O/data/michel_primaries/Sim_D2ODetector001.root");

    TChain* Sim_Tree = new TChain("Sim_Tree", "Sim_Tree");
    Sim_Tree->Add(fname->Data());

    simEvent *ev = new simEvent();
    Sim_Tree->SetBranchAddress("eventData", &ev);

    // Output file and histograms
    TFile *outFile = new TFile("eventTime.root", "RECREATE");

    TH1D* D2O_dTime = new TH1D("D2O_time_window", "D2O_time_window", 100, 0, 1000);
    TH1D* H2O_dTime = new TH1D("H2O_time_window", "H2O_time_window", 100, 0, 1000);

    TH1D* D2O_coinc = new TH1D("D2O_coincidence", "D2O_coincidence", 13, 0, 13);
    TH1D* H2O_coinc = new TH1D("H2O_coincidence", "H2O_coincidence", 13, 0, 13);

    TH2D* D2O_dTime_vs_coinc = new TH2D("D2O_dTime_vs_coinc", "D2O_dTime_vs_coinc", 13, 0, 13, 100, 0, 1000);
    TH2D* H2O_dTime_vs_coinc = new TH2D("H2O_dTime_vs_coinc", "H2O_dTime_vs_coinc", 13, 0, 13, 100, 0, 1000);

    // Event loop
    for (int iEvent=0; iEvent < Sim_Tree->GetEntries(); iEvent++) {
        Sim_Tree->GetEntry(iEvent);

	int nPE = ev->numHits;
	int vol0 = ev->vol0;

        std::vector<int> hits(12, 0);  // initialize vector with size 12, each element in the vector will have info of numHits in the PMT number, initialize the vector with 0's for now


	int coinc = 0;
	double minEventTime = 10000.;
	double maxEventTime = 0.;

	// Calculate coincidence and timing window
	if (nPE > 0) {
            for (int iHit = 0; iHit < nPE; iHit++) {
                const simHit *hit = ev->GetHit(iHit);
                int pmtNum = hit->pmtNum;
		hits[pmtNum] += 1;

		double eventTime = hit->eventTime; 

		if (eventTime <= minEventTime) minEventTime = eventTime;
		if (eventTime >= maxEventTime) maxEventTime = eventTime;
            }

            coinc = hits.size() - count(hits.begin(), hits.end(), 0);

	    double dEventTime = maxEventTime - minEventTime;

	    // Fill the histograms for D2O and H2O
	    if (vol0 == 1) {
	        D2O_dTime->Fill(dEventTime);
	        D2O_coinc->Fill(coinc);
                D2O_dTime_vs_coinc->Fill(coinc, dEventTime);
	    }
	    else if (vol0 == 2) {
	        H2O_dTime->Fill(dEventTime);
	        H2O_coinc->Fill(coinc);
                H2O_dTime_vs_coinc->Fill(coinc, dEventTime);
	    }
        }
    }
    
    // Set axes
    D2O_dTime->GetYaxis()->SetTitle("Events / bin");
    H2O_dTime->GetYaxis()->SetTitle("Events / bin");

    D2O_dTime->GetXaxis()->SetTitle("Time Interval between First and Last PMT Hits");
    H2O_dTime->GetXaxis()->SetTitle("Time Interval between First and Last PMT Hits");
 
    // Write to file
    D2O_dTime->Write();
    H2O_dTime->Write();

    D2O_coinc->Write();
    H2O_coinc->Write();

    // Normalize to unity
    D2O_dTime_vs_coinc->Scale(1./D2O_dTime_vs_coinc->GetEntries());
    H2O_dTime_vs_coinc->Scale(1./H2O_dTime_vs_coinc->GetEntries());

    // Some drawing options
    D2O_dTime_vs_coinc->SetMaximum(0.09);
    H2O_dTime_vs_coinc->SetMaximum(0.09);

    D2O_dTime_vs_coinc->SetStats(0);
    H2O_dTime_vs_coinc->SetStats(0);

    D2O_dTime_vs_coinc->SetOption("colz");
    H2O_dTime_vs_coinc->SetOption("colz");

    D2O_dTime_vs_coinc->Write();
    H2O_dTime_vs_coinc->Write();

    outFile->Close();

}
