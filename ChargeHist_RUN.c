void ChargeHist_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/ChargeHist.cpp");
    gROOT->ProcessLine("ChargeHist()");

}

