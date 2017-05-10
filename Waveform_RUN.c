// Sintassi corretta: root nomefile.root -l Waveform_RUN.c
void Waveform_RUN(int PMTid){
    gInterpreter->AddIncludePath("src/");
    gInterpreter->AddIncludePath("include/");
    gROOT->ProcessLine(".L src/Waveform.cpp");
	char tempname[250];
	sprintf(tempname,"Waveform(%d);",PMTid);
    gROOT->ProcessLine(tempname);
	//Waveform(PMTid);
}
