#include "universe.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
};

//Constructor function that creates the universe
//First Parameter: # of rows
//Second Parameter: # of colums
//Third Parameter: Whether toroidal is true or not
Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    Universe *uv = (Universe *) malloc(sizeof(Universe));
    if (uv != NULL) {
        uv->rows = rows;
        uv->cols = cols;
        uv->toroidal = toroidal;
        bool **grid = (bool **) calloc(rows, sizeof(bool *));
        for (uint32_t r = 0; r < rows; r += 1) {
            grid[r] = (bool *) calloc(cols, sizeof(bool));
        }
        uv->grid = grid;
        if (!uv->cols) {
            free(uv);
            uv = NULL;
        }
    }
    return uv;
}

//Destructor function that frees any memory allocated for the Universe
//First parameter: Pointer to the Universe struct
void uv_delete(Universe *u) {
    for (uint32_t i = 0; i < (u->rows); i++) {
        free(u->grid[i]);
    }
    free(u->grid);
    free(u);
    return;
}

//Accessor function that returns the number of rows in the Universe
//First Parameter: Pointer to a Universe struct
uint32_t uv_rows(Universe *u) {
    uint32_t rows = u->rows;
    return rows;
}

//Accessor function that returns the number of cols in the Universe
//First Parameter: Pointer to a Universe struct
uint32_t uv_cols(Universe *u) {
    uint32_t cols = u->cols;
    return cols;
}

//Manipulator function that makes a cell come to life (Sets bool=true)
//First parameter: Pointer to a Universe struct
//Second parameter: Row of the cell that should come to life
//Third parameter: Column of the cell that should come to life
void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {

    if (r >= 0 && r < uv_rows(u)) {
        if (c >= 0 && c < uv_cols(u)) {
            u->grid[r][c] = true;
        }
    }
    return;
}

//Manipulator function that makes a cell die (Sets bool=true)
//First parameter: Pointer to a Universe struct
//Second parameter: Row of the cell that should die
//Third parameter: Column of the cell that should die
void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r >= 0 && r < uv_rows(u)) {
        if (c >= 0 && c < uv_cols(u)) {
            u->grid[r][c] = false;
        }
    }
    return;
}

//Accessor function that returns the state of a cell (false is returned if out of bounds)
//First parameter: Pointer to a Universe struct
//Second parameter: Row of the cell whose status should be returned
//Third parameter: Column of the cell whose status should be returned
bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    if (r >= 0 && r < uv_rows(u)) {
        if (c >= 0 && c < uv_cols(u)) {
            return u->grid[r][c];
        }
    }
    return false;
}

//Manipulator function that populates a universe with live cells
//First parameter: Pointer to a Universe struct
//Second parameter: Pointer to a file (File you want to read from)
bool uv_populate(Universe *u, FILE *infile) {
    uint32_t row, col;
    while (fscanf(infile, "%" SCNu32 " %" SCNu32 "\n", &row, &col) != EOF) {
        if (row < 0 || row > (u->rows - 1) || col < 0 || col > (u->cols - 1)) {
            fprintf(stderr, "Column or Row is out of bounds (%u,%u)\n", row, col);
            exit(false);

        } else {
            uv_live_cell(u, row, col);
        }
    }
    return true;
}

//Accessor function that returns the # of live cells around the inputed cell (Can be toroidal)
//First parameter: Pointer to a Universe struct
//Second parameter: Row coordinate of the cell
//Third parameter: Column coordinate of the cell
uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t live_neighbors = 0;
    if (r >= 0 && r < uv_rows(u)) {
        if (c >= 0 && c < uv_cols(u)) {
            if (r == 0 && c == 0) { //Test case for top left corner of grid
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[r + 1][c + (u->cols - 1)] == true) { //bot left
                        live_neighbors++;
                    }
                    if (u->grid[r][c + (u->cols - 1)] == true) { //left
                        live_neighbors++;
                    }
                    if (u->grid[r + (u->rows - 1)][c + (u->cols - 1)] == true) { //top left
                        live_neighbors++;
                    }
                    if (u->grid[r + (u->rows - 1)][c] == true) { //top
                        live_neighbors++;
                    }
                    if (u->grid[r + (u->rows - 1)][c + 1] == true) { //top right
                        live_neighbors++;
                    }
                }
                return live_neighbors;
            }
            if (r == (u->rows - 1) && c == 0) { //Test case for bottom left corner of grid
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c + 1]) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[(r - u->rows) + 1][c + 1] == true) { //Bot right
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][c] == true) { //Bottom
                        live_neighbors++;
                    }
                    if (u->grid[r - (u->rows) + 1][(c + u->cols) - 1] == true) { //Bot left
                        live_neighbors++;
                    }
                    if (u->grid[r][c + u->cols - 1] == true) { //Left
                        live_neighbors++;
                    }
                    if (u->grid[r - 1][c + u->cols - 1] == true) { //Top left
                        live_neighbors++;
                    }
                }
                return live_neighbors;
            }
            if (r == 0 && c == (u->cols - 1)) { //Test case for top right corner of grid
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c - 1]) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[r + 1][(c - u->cols) + 1] == true) { //Bot right
                        live_neighbors++;
                    }
                    if (u->grid[r][(c - u->cols) + 1] == true) { //Right
                        live_neighbors++;
                    }
                    if (u->grid[(r + u->rows) - 1][(c - u->cols) + 1] == true) { //Top right
                        live_neighbors++;
                    }
                    if (u->grid[(r + u->rows) - 1][c] == true) { //Top
                        live_neighbors++;
                    }
                    if (u->grid[(r + u->rows) - 1][c - 1] == true) { //Top left
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            }
            if (r == (u->rows - 1)
                && c == (u->cols - 1)) { //Test case for bottom right corner of grid
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c - 1]) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[r - 1][(c - u->cols) + 1] == true) { //Top right
                        live_neighbors++;
                    }
                    if (u->grid[r][(c - u->cols) + 1] == true) { //Right
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][(c - u->cols) + 1] == true) { //Bot right
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][c] == true) { //Bottom
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][c - 1] == true) { //Bot left
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            }
            if (r == 0 && c > 0 && c < u->cols) { //Test case for top of grid
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[(r + u->rows) - 1][c + 1] == true) { //Top Right
                        live_neighbors++;
                    }
                    if (u->grid[(r + u->rows) - 1][c] == true) { //Top
                        live_neighbors++;
                    }
                    if (u->grid[(r + u->rows) - 1][c - 1] == true) { //Top Left
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            }

            if (r > 0 && r < u->rows && c == u->cols - 1) { //Test case for right of grid
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[r + 1][(c - u->cols) + 1] == true) { //Bot Right
                        live_neighbors++;
                    }
                    if (u->grid[r][(c - u->cols) + 1] == true) { //Right
                        live_neighbors++;
                    }
                    if (u->grid[r - 1][(c - u->cols) + 1] == true) { //Top right
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            }

            if (r == u->rows - 1 && c > 0 && c < u->cols) { //Test case for bottom of grid
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[(r - u->rows) + 1][c + 1] == true) { //Bot Right
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][c] == true) { //Bottom
                        live_neighbors++;
                    }
                    if (u->grid[(r - u->rows) + 1][c - 1] == true) { //Bot left
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            }

            if (r > 0 && r < u->rows && c == 0) { //Test case for left of grid
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->toroidal == true) {
                    if (u->grid[r + 1][(c + u->cols) - 1] == true) { //Bot left
                        live_neighbors++;
                    }
                    if (u->grid[r][(c + u->cols) - 1] == true) { //Left
                        live_neighbors++;
                    }
                    if (u->grid[r - 1][(c + u->cols) - 1] == true) { //Top left
                        live_neighbors++;
                    }
                    return live_neighbors;
                }
                return live_neighbors;
            } else { //Checking every other cell that is not a corner
                if (u->grid[r - 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c + 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c] == true) {
                    live_neighbors++;
                }
                if (u->grid[r + 1][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r][c - 1] == true) {
                    live_neighbors++;
                }
                if (u->grid[r - 1][c - 1] == true) {
                    live_neighbors++;
                }
                return live_neighbors;
            }
        }
    }
    return live_neighbors;
}

//Accessor function that outputs the current universe to a file
////First parameter: Pointer to a Universe struct
////Second parameter: Pointer to a file (File you want to print to)
void uv_print(Universe *u, FILE *outfile) {
    for (uint32_t x = 0; x < u->rows; x++) {
        for (uint32_t y = 0; y < u->cols; y++) {
            if (u->grid[x][y] == true) {
                fprintf(outfile, "o");
            } else {
                fprintf(outfile, ".");
            }
        }
        fprintf(outfile, "\n");
    }
    return;
}
