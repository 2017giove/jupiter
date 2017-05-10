// Sintassi corretta: root nomefile.root -l Waveform_RUN.c
void WaveformCharge_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Waveform.cpp");
    gROOT->ProcessLine("plotWaveStepCharge();");

}
