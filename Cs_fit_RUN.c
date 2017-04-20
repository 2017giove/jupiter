// Sintassi corretta da terminale: root nomefile.root -l ChargeHist_RUN.c
void Cs_fit_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Cs_fit.cpp");
    gROOT->ProcessLine("Cs_fit()");

}

