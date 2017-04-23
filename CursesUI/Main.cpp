
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <panel.h>
#include <pthread.h>
#include "TCPClient.hpp"




uint8_t         data[1024*4];
bool            runFlag     = true;
uint32_t        scale       = 1;
uint32_t        offset      = 0;


//
//
//
void* DataUpdateThread(void*)
{
    TCPClientConnect((char*)"192.168.1.141",1234);
    while(true)
    {
        if(runFlag == true)
        {

            //
            // Get some new data.
            //
#if 0
            static uint32_t     iteration   = 0;
            iteration++;
            for(uint32_t k=0; k<sizeof(data)-1; k++)
            {
                data[k] = data[k+1];
            }
            if(iteration%5 == 0)
            {
                data[sizeof(data)-1] = rand();
            }
#else
            TCPRead( &data[0], sizeof(data) );
#endif

        }

        //usleep(1000000/30);
    }
}



//
//
//
void DrawTraces(WINDOW* traceWin)
{
    int         maxX;
    int         maxY;


    wclear(traceWin);
    getmaxyx(traceWin, maxY,maxX);

    uint32_t traceHeight = maxY/8;
    for(uint32_t i=0; i<8; i++)
    {
        attron(COLOR_PAIR(i));

        //
        //
        //
        for(uint32_t j=0; j<maxX; j++)
        {
            uint32_t     sample  = 0;
            static uint32_t     previousSample  = sample;

            sample  = (data[j*scale+offset] & (1<<i)) != 0;


            int     ty  = (traceHeight*(i+1)) - 1 - (sample*(traceHeight-2));
            mvwaddch(traceWin, ty,j, '-');

            if((previousSample != sample) && (j>0))
            {
                ty  = (traceHeight*(i+1)) - 1;
                for(uint32_t k=0; k<traceHeight-2; k++)
                {
                    mvwaddch(traceWin, ty-k-1,j, '|');
                }
                mvwaddch(traceWin, ty,j, '+');
                mvwaddch(traceWin, ty-(traceHeight-2),j, '+');
            }

            previousSample  = sample;
        }

        attroff(COLOR_PAIR(i));
    }
}



void TogglePauseRun()
{
    if(runFlag == true)
    {
        runFlag = false;
    }
    else
    {
        runFlag = true;
    }
}


//
//
//
int main(void) 
{
    
    WINDOW*  mainwin;
    WINDOW*  childwin;
    WINDOW*  traceWin;
    PANEL*   mainPanel;
    PANEL*   childPanel;
    PANEL*   tracePanel;
    int      ch;


    //
    //
    //
    for(uint32_t i=0; i<sizeof(data); i++)
    {
        data[i] = rand();
    }

    //
    //
    //
    pthread_t   threadId;
    pthread_create(&threadId, NULL, DataUpdateThread, NULL);


    /*  Set the dimensions and initial
	position for our child window   */

    int      width = 23, height = 7;
    int      rows  = 25, cols   = 80;
    int      x = (cols - width)  / 2;
    int      y = (rows - height) / 2;


    /*  Initialize ncurses  */

    if ( (mainwin = initscr()) == NULL ) 
    {
	    fprintf(stderr, "Error initialising ncurses.\n");
	    exit(EXIT_FAILURE);
    }
    

    /*  Switch of echoing and enable keypad (for arrow keys)  */

    noecho();
    nodelay(mainwin, TRUE);
    keypad(mainwin, TRUE);


    /*  Make our child window, and add
	a border and some text to it.   */

    int         maxX;
    int         maxY;
    getmaxyx(mainwin, maxY,maxX);
    traceWin = newwin(maxY, maxX, 0, 0);

    childwin = newwin(height, width, 2, maxX-width-2);
    box(childwin, 0, 0);
    mvwaddstr(childwin, 1, 4, "Move the window");
    mvwaddstr(childwin, 2, 2, "with the arrow keys");
    mvwaddstr(childwin, 3, 6, "or HOME/END");
    mvwaddstr(childwin, 5, 3, "Press 'q' to quit");

    //
    //
    //
    start_color();
    init_pair(0, COLOR_RED, COLOR_BLACK);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);


    tracePanel  = new_panel( traceWin );
    mainPanel   = new_panel( mainwin );
    childPanel  = new_panel( childwin );

    uint32_t    wx   = 100;
    while (true)
    {
        //
        //
        //
        int ch  = getch();
        if(ch != ERR)
        {
            if(ch == 'q')
            {
                break;
            }

            if(ch == ' ')
            {
                TogglePauseRun();
            }

            if(ch == KEY_UP)
            {
                if(scale > 0)
                {
                    scale--;
                }
            }
            if(ch == KEY_DOWN)
            {
                scale++;
            }
            if(ch == KEY_LEFT)
            {
                if(offset > 0)
                {
                    offset-=scale;
                }
            }
            if(ch == KEY_RIGHT)
            {
                offset+=scale;
            }


            if(ch == KEY_NPAGE)
            {
                offset+=scale*10;
            }
            if(ch == KEY_PPAGE)
            {
                if(offset > scale*10)
                {
                    offset-=scale*10;
                }
                else
                {
                    offset  = 0;
                }
            }
        }

        //
        // Draw the traces.
        //
        DrawTraces(traceWin);

        //touchwin(childwin);
        //wnoutrefresh(traceWin);
        //wnoutrefresh(childwin);
        update_panels();
        doupdate();

        usleep(1000000/30);
    }


    /*  Clean up after ourselves  */

    delwin(childwin);
    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}
