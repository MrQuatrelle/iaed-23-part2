#ifndef MAIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STOP_NAME_LENGTH 50
#define LINE_NAME_LENGTH 20
#define MAX_INPUT 65535

#define DELIMITERS " \t\n"
#define INVERT "inverso"

typedef struct {
    double latitude;
    double longitude;
} location_t;

typedef struct line_node line_node_t;

typedef struct {
    char* name;
    location_t locale;
    int num_lines;
    line_node_t* head_lines;
} stop_t;

typedef struct stop_node {
    stop_t* raw;
    struct stop_node* next;
    struct stop_node* prev;
    double cost;
    double duration;
} stop_node_t;

typedef struct line {
    char* name;
    stop_node_t* origin;
    stop_node_t* destination;
    double total_cost;
    double total_duration;
    int num_stops;
} line_t;

struct line_node {
    line_t* raw;
    struct line_node* next;
};

#endif /* !MAIN_HEADER */
