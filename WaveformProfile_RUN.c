// Sintassi corretta: root nomefile.root -l Waveform_RUN.c
void WaveformProfile_RUN(){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Waveform.cpp");
	char tempname[250];

    gROOT->ProcessLine("WaveProfile()");
	//Waveform(PMTid);
}
