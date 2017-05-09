// Sintassi corretta: root nomefile.root -l Waveform_RUN.c
void Waveform_RUN(int PMTid){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Waveform.cpp");
    gROOT->ProcessLine("Waveform(PMTid);");
}
