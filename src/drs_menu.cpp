/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */


#ifndef J_MENU

#define J_MENU

#include "defines.h""
#include <ncurses.h>
#include <vector>
#include <string>


#include <unistd.h>

typedef struct ascimg {
    char **data;
    int rows;
} textimage;

textimage menu_getimage();
void menu_printtextimage(textimage img);

void mainmenu() {

    initscr(); /* Start curses mode 		  */
    start_color(); /* Start color mode */
    init_pair(1, COLOR_WHITE, COLOR_BLUE); /* Imposta un tema possibile */
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_CYAN);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_WHITE, COLOR_RED);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);
    init_pair(8, COLOR_BLACK, COLOR_YELLOW);
    attron(COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1)); /*Imposta il colore di sfondo */


    printw("GOOD PEOPLE!\n");
    printw("What i would like to say is \n");
    printw("Smearing\t e la scaldiamo con l'asciugacapelli\n");
    std::system("firefox https://goo.gl/KDcjdZ -fullscreen &");
    sleep(5);
    std::system("xdotool key 'F11'");

    int i = 1;
    while (1) {
        attron(COLOR_PAIR(i));
        bkgd(COLOR_PAIR(i));
        i++;
        if (i == 9) i = 1;
        usleep(50000);
        refresh();
    }
    // menu_printtextimage(menu_getimage());
    refresh(); /* Print it on to the real screen */
    getch(); /* Wait for user input */
    endwin();

}

WINDOW *w;
WINDOW *infow;

void menu_printsettings(mySetting* st) {
    int k = 0;
    mvwprintw(infow, k++, 2, "Data captured on %s \n", st->date);
    mvwprintw(infow, k++, 2, "for %d secs in configuration %d with %d channels\n", st->deltaT, st->triggerSetting, st->Nchan);

    mvwprintw(infow, k++, 2, "Delay ns = %d\n", st->delayns);
    mvwprintw(infow, k++, 2, "\n");
    int ii;
    for (ii = 0; ii < st->Nchan; ii++) {
        mvwprintw(infow, k++, 2, "CH #%d - PMT_ID =\t%d\n", ii, st->PmtID[ii]);
        mvwprintw(infow, k++, 2, "Voltage(V)\t=\t%4.0f ", st->voltage[ii]);
        mvwprintw(infow, k++, 2, "Threshold(mV)\t=\t%2.0f", st->thresh[ii]);
        mvwprintw(infow, k++, 2, "\n");
    }

    mvwprintw(infow, k++, 2, "Press ENTER to open TBrowser");
}

void filemenu(const char*filename) {


    TFile *sorgente_file = TFile::Open(filename);
    mySetting st;
    TTree* tset1 = (TTree*) sorgente_file->Get("tset");
    mySetting_get(tset1, &st);

    char tname[STR_LENGTH];
    bool printHistoAnalysis = 0;
    bool printAcqAnalysis = 0;

    sprintf(tname, "t1");
    TH1D *h0 = (TH1D*) sorgente_file->Get(tname);
    if (h0 != nullptr) {
        printAcqAnalysis = 1;
    }

    for (int j = 0; j < st.Nchan; j++) {
        int PMTid = CHtoPMT(j, &st);


        sprintf(tname, "h%d", PMTid);
        TH1D *h1 = (TH1D*) sorgente_file->Get(tname);
        if (h1 != nullptr) {
            printHistoAnalysis = 1;
        }



        sprintf(tname, "hc%d", PMTid);
        TH1D *h2 = (TH1D*) sorgente_file->Get(tname);
        if (h2 != nullptr) {
            printHistoAnalysis = 1;
        }
    }

    int MENU_INFORMATION = -1;
    int MENU_CHARGEHIST = -1;
    int MENU_CHARGEHISTRAW = -1;
    int MENU_CSFIT = -1;
    int MENU_WAVEFORM = -1;
    int MENU_WAVEPROFILE = -1;

    int n = 0;
    std::vector<std::string> myEntries;

    myEntries.push_back("Information");
    MENU_INFORMATION = n++;

    myEntries.push_back("-----------------");
    n++;

    if (printAcqAnalysis) {
        myEntries.push_back("ChargeHist");
        MENU_CHARGEHIST = n++;

        myEntries.push_back("ChargeHist (recreate)");
        MENU_CHARGEHISTRAW = n++;

        myEntries.push_back("WaveForm");
        MENU_WAVEFORM = n++;

        myEntries.push_back("WaveProfile");
        MENU_WAVEPROFILE = n++;
    }

    if (printHistoAnalysis) {
        myEntries.push_back("Cs_fit");
        MENU_CSFIT = n++;
    }

    char item[50];
    char temp[STR_LENGTH];
    int keypressed, choice = 0, width = 7;
    int row, col;
    getmaxyx(stdscr, row, col);
    WINDOW * mainw = initscr(); // initialize Ncurses
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLUE, COLOR_CYAN);

    attron(COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1)); /*Imposta il colore di sfondo */


    mvprintw(1, 1, "OPEN FILE: %s", filename);
    refresh();

    w = newwin(22, 25, 2, 1); // create a new window
    box(w, 0, 0); // sets default borders for the window

    infow = newwin(22, 53, 3, 25 + 3); // create a new window
    // box(infow, 0, 0); //



    wbkgd(w, COLOR_PAIR(1));
    wbkgd(infow, COLOR_PAIR(2));


    // now print all the menu items and highlight the first one
    for (choice = 0; choice < myEntries.size(); choice++) {
        if (choice == 0)

            wattron(w, A_STANDOUT); // highlights the first item.
        else
            wattroff(w, A_STANDOUT);
        sprintf(item, "%-7s", myEntries[choice].c_str());
        mvwprintw(w, choice + 1, 2, "%s", item);
    }

    menu_printsettings(&st);

    wrefresh(w); // update the terminal screen
    wrefresh(infow);

    choice = 0;
    noecho(); // disable echoing of characters on the screen
    keypad(w, TRUE); // enable keyboard input for the window.
    curs_set(0); // hide the default screen cursor.

    // get the input
    while ((keypressed = wgetch(w)) != '\n') {

        // right pad with spaces to make the items appear with even width.
        sprintf(item, "%-7s", myEntries[choice].c_str());
        mvwprintw(w, choice + 1, 2, "%s", item);

        // use a variable to increment or decrement the value based on the input.
        switch (keypressed) {
            case KEY_UP:
                choice--;
                choice = (choice < 0) ? myEntries.size() - 1 : choice;
                break;
            case KEY_DOWN:
                choice++;
                choice = (choice >= myEntries.size()) ? 0 : choice;
                break;
        }
        // now highlight the next item in the list.
        wattron(w, A_STANDOUT);

        sprintf(item, "%-7s", myEntries[choice].c_str());
        mvwprintw(w, choice + 1, 2, "%s", item);

        wattroff(w, A_STANDOUT);

        // std::cout << "Choice is "<< choice <<std::endl;


        wclear(infow);
        if (choice == MENU_INFORMATION) {
            menu_printsettings(&st);
        } else if (choice == MENU_CHARGEHIST) {
            int k = 2;
            waddstr(infow, "Integra il segnale acquisito e crea l'istogramma della carica\n");

        } else if (choice == MENU_CHARGEHISTRAW) {
            int k = 2;
            waddstr(infow, "Integra il segnale acquisito e crea l'istogramma della carica\n");
            waddstr(infow, "Cancella l'eventuale istogramma precedentemente creato\n");
        } else if (choice == MENU_CSFIT) {

            int k = 2;
            waddstr(infow, "Fit dei dati con curva prevista per il Cesio\n");
        } else if (choice == MENU_WAVEFORM) {
            int k = 2;
            waddstr(infow, "Crea l'istogramma di ogni forma d'onda acquisita\n");
        } else if (choice == MENU_WAVEPROFILE) {
            int k = 2;
            waddstr(infow, "Analisi delle forme d'onda al variare della carica totale portata");
        }





        //   box(infow, 0, 0);
        wrefresh(w); // update the terminal screen
        wrefresh(infow);


    }


    delwin(w);
    endwin();


    if (choice == MENU_INFORMATION) {
        TBrowser * rch = new TBrowser();
        gSystem->ProcessEvents();
        int targc = 0;
        char* targv[50];
        TApplication theApp("App", &targc, targv);
        theApp.Run();
    } else if (choice == MENU_CHARGEHIST) {
        ChargeHist(filename);

    } else if (choice == MENU_CHARGEHISTRAW) {
        char fileRAWname[STR_LENGTH];
        sprintf(fileRAWname, "%sRAW.root", filenameFromPath(filename).c_str());
        std::vector<std::string> myrottenhist = list_files("data/", fileRAWname, "");
        removeFileList(myrottenhist);
        ChargeHist(filename);

    } else if (choice == MENU_CSFIT) {
        Cs_fitall(filename);

    } else if (choice == MENU_WAVEFORM) {
        WaveformAll(filename);
    } else if (choice == MENU_WAVEPROFILE) {
        WaveProfile(filename);
    }





}

void menu_printtextimage(textimage img) {

    int i;
    for (i = 0; i < img.rows; i++) {

        mvwprintw(stdscr, i + 5, 2, "%s\n", img.data[i]);

    }

    //  printf("\n");

}

textimage menu_getimage() {
    static char *lines[] = {
        "  .-''`````````''-.	 ",
        " /'-.___________.-'\	 ",
        ";                   ;	 ",
        "|    __.......__    |   ",
        "|.-'`      ~    `'-.|	",
        "|::-.___~______~.-'`|	 ",
        ";:::::.             ;	 ",
        " \::::.            /	 ",
        "  ':::::.        .'	",
        "    `-:::::.._.-'		 ",
        "         | |			 ",
        "         | |			 ",
        "         | |			 ",
        "         | |			 ",
        "         | |			 ",
        "         | |			 ",
        "      ___| |___		 ",
        "  .-'`   | |   `'-.	",
        " (      `''`      )	 ",
        "  '._           _.'	 ",
        "     `'''''''''`		"
    };
    return (textimage){ lines, sizeof (lines) / sizeof (char*)};
}


#endif