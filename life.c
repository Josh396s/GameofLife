#include "universe.h"

#include <ctype.h>
#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define DELAY 50000

void print_uni(Universe *u); //TEST FUNCTION DECLARATION (USED TOL PRINT THE UNIVERSE)
void usage();

int main(int argc, char *argv[]) {
    int opt = 0;
    uint32_t gen = 100;
    char *input_file;
    char *output_file;
    FILE *infile = stdin;
    FILE *outfile = stdout;

    bool toroidal = false;
    bool ncurses = true;
    bool case1 = false;
    bool case2 = false;
    bool case3 = false;

    while ((opt = getopt(argc, argv, "tsHn:i:o:")) != -1) {
        switch (opt) {
        case 't': case1 = true; break;
        case 's': case2 = true; break;
        case 'n':
            gen = atoi(optarg);
            uint32_t check = gen;
            if (check == 0) {
                fprintf(stderr, "Please enter a valid number of generations \n");
                return 1;
            } else {
                continue;
            }
        case 'i': input_file = optarg;
        case 'o': output_file = optarg;
        case 'H': case3 = true; break;
        default: fprintf(stderr, "Error\n"); return 1;
        } //END switch

        if (case1) {
            toroidal = true; //Universe is toroidal
        } //End case1
        if (case2) {
            ncurses = false; //Silence ncurses
        } //End case2
    } //END getopt()
    uint32_t row, col;
    infile = fopen(input_file, "r");
    if (infile == NULL) {
        fprintf(stderr, "File does not exist\n");
        return 1;
    }
    fscanf(infile, "%" SCNu32 "%" SCNu32 "\n", &row, &col); //####2 READ NUMBER OF ROWS/COLS
    Universe *A = uv_create(row, col, toroidal); //####3 CREATE 2 UNIVERSES
    Universe *B = uv_create(row, col, toroidal); //####3 CREATE 2 UNIVERSES
    uv_populate(A, infile); //####4 POPULATE UNIVERSE A
    fclose(infile); //Close input file
    initscr();
    curs_set(FALSE);

    for (uint32_t counter = 0; counter <= gen; counter++) {
        clear();
        for (uint32_t x = 0; x < uv_rows(A); x++) {
            for (uint32_t y = 0; y < uv_cols(A); y++) {
                if (uv_get_cell(A, x, y) == true) {
                    mvprintw(x, y, "o");
                }
            }
        }
        if (ncurses) {
            refresh();
            usleep(DELAY);
        }
        for (uint32_t x = 0; x < uv_rows(A); x++) {
            for (uint32_t y = 0; y < uv_cols(A); y++) {
                if (uv_get_cell(A, x, y) == true) { //Rule #1
                    if (uv_census(A, x, y) == 2 || uv_census(A, x, y) == 3) { //Inner if
                        //              printf("Alive Cell: (%u, %u)", x, y);
                        uv_live_cell(B, x, y); //Set the cell alive in Universe B
                    } //End inner if
                    else {
                        uv_dead_cell(B, x, y);
                    }
                } //End if (Rule #1)
                if (uv_get_cell(A, x, y) == false) {
                    if (uv_census(A, x, y) == 3) { //Rule #2
                        //                 printf("Dead Cell come Alive: (%u, %u)", x, y);
                        uv_live_cell(B, x, y);
                    } else {
                        uv_dead_cell(B, x, y);
                    }
                } //End if (Rule#2)
            } //End for loop (cols)
        } //End for loop(rows)
        Universe *temp = A;
        A = B;
        B = temp;
    } //End generation for loop
    endwin();
    if (output_file != input_file) {
        outfile = fopen(output_file, "w");
        uv_print(B, outfile);
        fclose(outfile);
    } else {
        uv_print(B, outfile);
    }
    uv_delete(A);
    uv_delete(B);
    return 0;
} //End case1
