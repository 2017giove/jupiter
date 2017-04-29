//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Apr 29 11:21:04 2017 by ROOT version 5.34/28
// from TTree tset/Acquire Settings
// found on file: data/nataleacaprera_405.root
//////////////////////////////////////////////////////////

#ifndef tset_h
#define tset_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class tset {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Delay_ns;
   Char_t          Date[26];
   Float_t         Voltage;
   Int_t           PMT_ID;
   Float_t         threshold;

   // List of branches
   TBranch        *b_Delay_ns;   //!
   TBranch        *b_date;   //!
   TBranch        *b_Voltage;   //!
   TBranch        *b_PMT;   //!
   TBranch        *b_thresh;   //!

   tset(TTree *tree=0);
   virtual ~tset();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tset_cxx
tset::tset(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("data/nataleacaprera_405.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("data/nataleacaprera_405.root");
      }
      f->GetObject("tset",tree);

   }
   Init(tree);
}

tset::~tset()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t tset::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t tset::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void tset::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Delay_ns", &Delay_ns, &b_Delay_ns);
   fChain->SetBranchAddress("Date", Date, &b_date);
   fChain->SetBranchAddress("Voltage", &Voltage, &b_Voltage);
   fChain->SetBranchAddress("PMT_ID", &PMT_ID, &b_PMT);
   fChain->SetBranchAddress("threshold", &threshold, &b_thresh);
   Notify();
}

Bool_t tset::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void tset::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t tset::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef tset_cxx
