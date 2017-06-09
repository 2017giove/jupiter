#define Waveform_cxx
#include "Waveform.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>

void Waveform::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L Waveform.C
//      Root > Waveform t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();



   TH1F histo_ch1("histo_ch1","histo_ch1",1024,0,1024);
   TH1F histo_max("histo_max","histo_max",100,0,4024);

   //TH1F histo_ch2("histo_ch2","histo_ch2",1024,0,1024);
   //TH1F histo_ch3("histo_ch3","histo_ch3",1024,0,1024);
   //TH1F histo_ch4("histo_ch4","histo_ch4",1024,0,1024);
   TF1 *gf = new TF1("f1", "([0]*TMath::Exp(-[1]*(x-[3])) - [4]*TMath::Exp(-[2]*(x-[5])))", 200,1024); 

   int wait;
   Long64_t minimo;
   
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
        fChain->GetEntry(jentry);
     //nb = fChain->GetEntry(jentry);   nbytes += nb;
     // if (Cut(ientry) < 0) continue;

        gf->SetParameter(0,1620);
        gf->FixParameter(1,0.00867);
        gf->FixParameter(2,0.0992);
        gf->SetParameter(3,220);
        gf->SetParameter(4,1620);
        gf->SetParameter(5,220);
        
     for (int k=0; k<1024; k++)
       {	
	 histo_ch1.SetBinContent(k,wave_array[0][k]);	 
	 //histo_ch2.SetBinContent(k,wave_array[1][k]);
	 //histo_ch3.SetBinContent(k,wave_array[2][k]);
	 //histo_ch4.SetBinContent(k,wave_array[3][k]);
       }
          
     TCanvas c;
     c.Draw();
     histo_ch1.Draw();
          c.Update();
     
     
    //       histo_ch1.Rebin(8);
     TH1F temp = histo_ch1;
     temp.Rebin(8);
     temp.Fit(gf,"Q","",215,1024);  // fit LOLLO ROSSone
     
     minimo= -gf.GetMinimum(215,1024);
             
     //printf("\n%f\n",minimo);
     histo_max.Fill(minimo,1); 
     
     
     //if(wait==0)
     //  break;

   }
   

     c.Update();
     
     
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     fChain->GetEntry(jentry);  
     
   }

    cin.clear();
     cin >> wait ;
   
   TFile f("outputfile.root","RECREATE");
   histo_max.Write();
   f.Close();

}