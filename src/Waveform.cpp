#define Waveform_cxx
#include "Waveform.h"

int Waveform::FittingStartBin(float threshold, TH1F * hist) {
    int i;
    if (threshold < 0) {
        for (i = 0; i < hist->GetSize(); i++) {
            if (hist->GetBinContent(i) < threshold) return i - 1;
        }
        return 0;
    } else if (threshold >= 0) {
        for (i = 0; i < hist->GetSize(); i++) {
            if (hist->GetBinContent(i) > threshold) return i - 1;
        }
        return 0;
    }
}

void Waveform::Loop() {

    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();
    Long64_t minimo;

    TCanvas *c = new TCanvas("cA", PLOTS_TITLE, 640, 480);
    TCanvas *c2 = new TCanvas("cB", PLOTS_TITLE, 640, 480);
    //TPad* pad1 = new TPad("pad1","The pad",0.03,0.62,0.50,0.92,21);

    TH1F *histo_ch1 = new TH1F("histo_ch1", "Forma del segnale", 1024, 0, N_SAMPLES);
    TH1F *histo_max = new TH1F("histo_max", "Istogramma dei massimi", 500, 0, 5024);
    TF1 *gf = new TF1("f1", "([0]*TMath::Exp(-[1]*(x-[3])) - [4]*TMath::Exp(-[2]*(x-[5])))", 0, N_SAMPLES);

    TH1F *showHist;
    TF1 *showFit;

    TTree* tset1 = (TTree*) f->Get("tset");
    float thres;
    tset1->SetBranchAddress("threshold", &thres);
    tset1->GetEntry();

    // printf("Lorenzo ha detto %f\n", thres);

    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        fChain->GetEntry(jentry);
        //nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) conttensioneinue;

        gf->SetParameter(0, 1620);
        gf->FixParameter(1, 0.00867);
        gf->FixParameter(2, 0.0992);
        gf->SetParameter(3, 220);
        gf->SetParameter(4, 1620);
        gf->SetParameter(5, 220);

        for (int k = 0; k < 1024; k++) {
            histo_ch1->SetBinContent(k, wave_array[0][k]);
        }


        TH1F *temp = (TH1F*) histo_ch1->Clone("GrongoHist");
        temp ->Rebin(16);
        for (int k = 0; k < 64; k++) {
            temp->SetBinContent(k, temp->GetBinContent(k) / 16);
        }

        // printf("Lorenzo ha detto %f\n", FittingStartBin(thres,temp));
        // se vuoi velocizzare parti da start=(N_SAMPLES - (int) (delay * RATE))
        temp->Fit(gf, "Q", "", FittingStartBin(thres, histo_ch1), N_SAMPLES);
        //
        //
        //        showHist = (TH1F*) temp->Clone("GrongoWave");
        //        showFit = (TF1*) gf->Clone("GrongoCurve");
        //        printf("Pesco un granchio...\n");
        //        c2->cd();
        //        showHist->Draw();
        //        showFit->Draw("same");
        //        c2->Update();

        if (jentry % 10000 == 0) {
            showHist = (TH1F*) histo_ch1->Clone("GrongoWave");
            showFit = (TF1*) gf->Clone("GrongoCurve");
            printf("Pesco un granchio...\n");
            c2->cd();
            showHist->GetXaxis()->SetTitle("tempo (samples)");
            showHist->GetYaxis()->SetTitle("Segnale (mV)");
            //
            //            showHist->SetMinimum(-800);
            //            showFit->SetMinimum(-800);
            //            showHist->SetMaximum(50);
            //            showFit->SetMaximum(50);
            //

            //showFit->DrawF1(0, N_SAMPLES, "pl same");
            // showHist->(0, -800, N_SAMPLES, 50);
            //showFit->SetRange(0, -800, N_SAMPLES, 50);

            showHist->Draw();
            showFit->Draw("same");
            c2->Update();
            std::fflush(stdout);
        }

        minimo = -gf->GetMinimum(0, N_SAMPLES);
        histo_max->Fill(minimo, 1);

        printf("%d/%d\t", jentry, nentries);
        printStatus((float(jentry) / (float) nentries));
    }

    c2->cd();
    showHist->Draw();
    //showFit->DrawF1(0, N_SAMPLES, "pl same");
    showFit->Draw("pl same");

    c->cd();
    //histo_max->Rebin();
    histo_max->GetXaxis()->SetTitle("Qualcosa proporzionale alla corrente");
    histo_max->GetYaxis()->SetTitle("# eventi");
    //histo_max->SetFillColor(kYellow);
    histo_max->Draw("Hist");
    c2->Update();
    //    
    //TPaveStats *stats = (TPaveStats *)histo_max->FindObject("stats");

    // c2->Update();
    // TFile f("outputfile.root","RECREATE");
    // histo_max.Write();
    //f.Close();

}


