// Sintassi corretta da terminale: root nomefile.root -l ChargeHist_RUN.c
void ChargeHist_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/ChargeHist.cpp");
    gROOT->ProcessLine("ChargeHist()");

}

