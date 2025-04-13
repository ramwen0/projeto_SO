#ifndef INPUTS_H
#define INPUTS_H

#define NUM_INPUTS 6

extern int input00[5][20];
extern int input01[5][20];
extern int input02[4][20];
extern int input03[5][20];
extern int input04[11][20];
extern int input05[11][20];

typedef struct {
    int (*programs)[20];
    int rows;
} SimulationInput;

#endif