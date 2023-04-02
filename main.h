#ifndef MAIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STOP_NAME_LENGTH 50
#define LINE_NAME_LENGTH 20

#define DELIMITERS " \t\n"
#define INVERT "inverso"

typedef struct {
    double latitude;
    double longitude;
} location_t;

typedef struct {
    char name[STOP_NAME_LENGTH + 1];
    location_t locale;
    int num_lines;
} stop_t;

typedef struct stop_node {
    stop_t* raw;
    struct stop_node* next;
    struct stop_node* prev;
} stop_node_t;

typedef struct line {
    char name[LINE_NAME_LENGTH + 1];
    stop_node_t* origin;
    stop_node_t* destination;
    double total_cost;
    double total_duration;
    int num_stops;
} line_t;

#endif /* !MAIN_HEADER */
