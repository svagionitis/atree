/*Build: gcc -std=c99 -o atree atree.c -lncurses*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <curses.h>

#define PI 3.14159265

double dx = 1, dy = 1, dz = 1;
double xScale = 1.0, yScale = 1.0, zScale = 1.0;

void usage(char *argv0)
{
    printf("USAGE: %s [height] [half|full]\n", argv0);
    exit(-1);
}

int color_str(int y, int x, short fg_color, short bg_color, const char * str)
{
    short i;
    // Search all the pair of colors
    // to match with the given one.
    // Then apply the specific pair.
    // Naive way
    for (i = 1;i < COLOR_PAIRS;i++)
    {
        short f, b;
        pair_content(i, &f, &b);
        if (f == fg_color && b == bg_color)
            break;
    }

    attron(COLOR_PAIR(i));

    mvaddstr(y,x,str);

    attroff(COLOR_PAIR(i));
    return 0;
}


void createSpiral(double i, short fg, short bg, char isLeft, double yLocalScale)
{
    double sign = isLeft ? -1.0 : 1.0;
    short _fg = fg, _bg = bg;
    if (!isLeft)
    {
        _fg = bg;
        _bg = fg;
    }

    double zoff = i * sin(i);

    int z = (dz / (dz - sign * zoff * zScale));
    int x = (sign * zoff * z * xScale + dx);
    int y = ((i * yLocalScale) * z * yScale + dy);


    if (zoff + sign * PI / 4.0 < 0.0)
        color_str(y, x, _fg, COLOR_BLACK, "*");
    else
        color_str(y, x, _bg, COLOR_BLACK, "*");


    //printf("i: %f x: %d, y:%d, z:%d\n", i, x, y, z);

    //color_str(y, x, COLOR_WHITE, COLOR_BLACK, "*");
    refresh();
}

void tree(char isLeft, double startFrom, double max)
{
    for (double i = startFrom;i < max + startFrom; i += 0.1)
    {
        if (i < 0.0 || i > max)
            continue;

        createSpiral(i, COLOR_WHITE, COLOR_BLACK, isLeft, 1.0);
    }
}

int main(int argc, char *argv[])
{
    WINDOW *mainwin;
    int ch;
    int maxY = 0, maxX = 0;
    double startFrom = 0.0;
    double max = 19.0;
    char isLeft = 1;

    ///*
    mainwin = initscr();
    if (mainwin == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(-1);
    }

    // Get the maximum size of the screen
    getmaxyx(mainwin, maxY, maxX);

    // Check if colors are supported
    if (!has_colors())
    {
        delwin(mainwin);
        endwin();
        fprintf(stderr,"Your terminal does not support color\n");
        exit(-1);
    }
    else
    {
        // Initialize colors
        start_color();

        // Assign terminal default foreground/background colors to color number -1
        use_default_colors();

        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
        init_pair(8, COLOR_RED,     COLOR_WHITE);
        init_pair(9, COLOR_GREEN,   COLOR_WHITE);
    }


    clear();

    // Turn off key echoing
    noecho();
    // Line buffering disabled
    cbreak();
    // This determines the visibility of the cursor.
    curs_set(FALSE);
    // Tell curses not to do NL->CR/NL on output
    nonl();
    // Enable the keypad for non-char keys
    keypad(mainwin, TRUE);

    //*/

    tree(isLeft, startFrom, max);

    ///*
    // Loop until press ESC
    while ((ch = getch()) != 27)
    {
        switch(ch)
        {
            case '0': // Set foreground/background colors to default
            case '1': // Set foreground/background colors according to init_pairs
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                attrset(COLOR_PAIR(ch - '0'));
                break;
            case 'l':
                if (isLeft)
                    isLeft = 0;
                else
                    isLeft = 1;
                break;
            case 'q':
                dx += 0.1;

                if (dx > maxX)
                    dx = maxX;
                break;
            case 'w':
                dy += 0.1;

                if (dy > maxY)
                    dy = maxY;
                break;
            case 'e':
                dz += 0.1;

                break;
            case KEY_UP: // Press Up to increase height
                break;
            case KEY_DOWN: // Press Down to decrease height
                break;
            case KEY_RIGHT: // Press right key reverse foreground/background colour.
                attron(A_REVERSE | A_BLINK);
                break;
            case KEY_LEFT: // Press left key return to default foreground/background colour.
                attroff(A_REVERSE | A_BLINK);
                break;
        }

        erase();
        tree(isLeft, startFrom, max);
    }


    delwin(mainwin);
    endwin();
    refresh();
    //*/

    return 0;
}

