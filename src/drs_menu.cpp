/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#include <ncurses.h>
#include <vector>
#include <string>

void mainmenu() {

    initscr(); /* Start curses mode 		  */
    start_color(); /* Start color mode */
    init_pair(1, COLOR_WHITE, COLOR_BLUE); /* Imposta un tema possibile */
    attron(COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1)); /*Imposta il colore di sfondo */


    printw("Welcome to the Space Time Continuum\n");

    refresh(); /* Print it on to the real screen */
    getch(); /* Wait for user input */
    endwin();

}

void filemenu(const char*filename) {

    WINDOW *w;

    std::vector<std::string> myEntries;
    myEntries.push_back("ChargeHist");
    myEntries.push_back("ChargeHist (new)");
    myEntries.push_back("Cs_fit");
    myEntries.push_back("WaveProfile");

    char item[50];
    int ch, i = 0, width = 7;

    initscr(); // initialize Ncurses
    w = newwin(14, 25, 1, 1); // create a new window
    box(w, 0, 0); // sets default borders for the window

    // now print all the menu items and highlight the first one
    for (i = 0; i < myEntries.size(); i++) {
        if (i == 0)
            wattron(w, A_STANDOUT); // highlights the first item.
        else
            wattroff(w, A_STANDOUT);
        sprintf(item, "%-7s", myEntries[i].c_str());
        mvwprintw(w, i + 1, 2, "%s", item);
    }

    wrefresh(w); // update the terminal screen

    i = 0;
    noecho(); // disable echoing of characters on the screen
    keypad(w, TRUE); // enable keyboard input for the window.
    curs_set(0); // hide the default screen cursor.

    // get the input
    while ((ch = wgetch(w)) != 'q') {

        // right pad with spaces to make the items appear with even width.
        sprintf(item, "%-7s", myEntries[i].c_str());
        mvwprintw(w, i + 1, 2, "%s", item);

        // use a variable to increment or decrement the value based on the input.
        switch (ch) {
            case KEY_UP:
                i--;
                i = (i < 0) ? myEntries.size() : i;
                break;
            case KEY_DOWN:
                i++;
                i = (i > myEntries.size()) ? 0 : i;
                break;
        }
        // now highlight the next item in the list.
        wattron(w, A_STANDOUT);

        sprintf(item, "%-7s", myEntries[i].c_str());
        mvwprintw(w, i + 1, 2, "%s", item);

        wattroff(w, A_STANDOUT);
    }

    delwin(w);
    endwin();

    if (i == 0) {
        ChargeHist(filename);
    } else if (i == 1) {

    } else if (i == 2) {
        Cs_fit(filename);
    }

}