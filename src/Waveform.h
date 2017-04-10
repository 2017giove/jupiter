//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Mar 14 15:12:21 2017 by ROOT version 5.34/28
// from TTree t1/title
// found on file: sorgente_10_03_2017.root
//////////////////////////////////////////////////////////
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

#ifndef Waveform_h
#define Waveform_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Waveform {
public:
    TTree *fChain; //!pointer to the analyzed TTree or TChain
    Int_t fCurrent; //!current Tree number in a TChain

    // Declaration of leaf types
    Int_t trigId;
    Int_t channels;
    Int_t id[4];
    Float_t time_array[4][1024];
    Float_t wave_array[4][1024];

    // List of branches
    TBranch *b_trigId; //!
    TBranch *b_channels; //!
    TBranch *b_id; //!
    TBranch *b_time_array; //!
    TBranch *b_wave_array; //!

    Waveform(const char *filename);
    Waveform();
    virtual ~Waveform();
    virtual Int_t Cut(Long64_t entry);
    virtual Int_t GetEntry(Long64_t entry);
    virtual Long64_t LoadTree(Long64_t entry);
    virtual void Init(TTree *tree);
    virtual void Loop();
    virtual Bool_t Notify();
    virtual void Show(Long64_t entry = -1);

};

#endif

#ifdef Waveform_cxx
//Waveform::Waveform(TTree *tree) : fChain(0) 

Waveform::Waveform(const char *filename) : fChain(0) {
    TTree *tree;
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    //if (tree == 0) {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->FindObject(filename);
    if (!f || !f->IsOpen()) {
        f = new TFile(filename);
    }
    f->GetObject("t1", tree);

    //}
    Init(tree);
}

Waveform::Waveform() : fChain(0) {
    TTree *tree;
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    //if (tree == 0) {
    TFile *f = (TFile*) gROOT->GetListOfFiles()->First();
    f->GetObject("t1", tree);

    //}
    Init(tree);
}

Waveform::~Waveform() {
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t Waveform::GetEntry(Long64_t entry) {
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}

Long64_t Waveform::LoadTree(Long64_t entry) {
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

void Waveform::Init(TTree *tree) {
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

    fChain->SetBranchAddress("trigId", &trigId, &b_trigId);
    fChain->SetBranchAddress("channels", &channels, &b_channels);
    fChain->SetBranchAddress("id", id, &b_id);
    fChain->SetBranchAddress("time_array", time_array, &b_time_array);
    fChain->SetBranchAddress("wave_array", wave_array, &b_wave_array);
    Notify();
}

Bool_t Waveform::Notify() {
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void Waveform::Show(Long64_t entry) {
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}

Int_t Waveform::Cut(Long64_t entry) {
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}
#endif // #ifdef Waveform_cxx
