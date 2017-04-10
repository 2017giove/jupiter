void Waveform_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Waveform.cpp");
    gROOT->ProcessLine("Waveform w");
    gROOT->ProcessLine("w.Loop();");
}
