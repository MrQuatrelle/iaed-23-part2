#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/cdefs.h>

line_t** lines;
stop_t** stops;

int stop_counter = 0;
int line_counter = 0;

/*
 * returns a pointer to the stop with the given name.
 * returns NULL if the stop doesn't exit.
 */
stop_t* get_stop(const char* name) {
    int i;
    for (i = 0; i < stop_counter; i++) {
        if (!strcmp(stops[i]->name, name))
            return stops[i];
    }
    return NULL;
}

/*
 * get the line identied by the given name.
 * returns NULL if it does not exist.
 */
line_t* get_line(const char* name) {
    int i;
    for (i = line_counter - 1; i >= 0; i--) {
        if (!strcmp(lines[i]->name, name))
            return lines[i];
    }
    return NULL;
}

/*
 * lists all lines in the system.
 */
void list_all_lines(void) {
    int i;
    for (i = 0; i < line_counter; i++) {
        printf("%s", lines[i]->name);
        if (lines[i]) {
            if (lines[i]->origin && lines[i]->destination) {
                printf(" %s %s", lines[i]->origin->raw->name,
                       lines[i]->destination->raw->name);
            }
            printf(" %d %.2f %.2f\n", lines[i]->num_stops, lines[i]->total_cost,
                   lines[i]->total_duration);
        }
    }
}

/*
 * lists the stops in the given line.
 */
void list_single_line(line_t* line) {
    stop_node_t* current = line->origin;

    if (current) {
        printf("%s", current->raw->name);
        current = current->next;
    }
    while (current) {
        printf(", %s", current->raw->name);
        current = current->next;
    }
    printf("\n");
}

/*
 * same as list_single_line() but inverted.
 */
void list_single_line_inverted(line_t* line) {
    stop_node_t* current = line->destination;

    if (current) {
        printf("%s", current->raw->name);
        current = current->prev;
    }
    while (current) {
        printf(", %s", current->raw->name);
        current = current->prev;
    }
    printf("\n");
}

void add_new_line(const char* name) {
    if (!(lines[line_counter] = (line_t*)malloc(sizeof(line_t)))) {
        printf("couldn't get memory for the new line!\n");
        fprintf(stderr, "maybe this should panic instead\n");
        return;
    }

    strcpy(lines[line_counter]->name, name);
    lines[line_counter]->total_cost = 0;
    lines[line_counter]->total_duration = 0;
    lines[line_counter]->origin = NULL;
    lines[line_counter]->destination = NULL;
    line_counter++;
}

/*
 * c command.
 * Adds a line to the system if one with the given name doesn't exist yet
 * Else, prints all the stops in the line.
 */
void list_or_add_line(char* str) {
    char* token;
    line_t* line;

    if (!(token = strtok(str, DELIMITERS))) {
        list_all_lines();
        return;
    }

    /* if the line already exists */
    if ((line = get_line(token))) {

        /* there is no sorting request */
        if (!(token = strtok(NULL, DELIMITERS))) {
            list_single_line(line);
            return;
        }
        /* in case there is, we have to verify its validity */
        if (!strncmp(token, INVERT, strlen(token))) {
            list_single_line_inverted(line);
            return;
        }
        printf("incorrect sort option.\n");
        return;
    }

    /* else add it */
    add_new_line(token);
}

/*
 * lists all the stops in the system.
 */
void list_all_stops(void) {
    int i;
    for (i = 0; i < stop_counter; i++) {
        printf("%s: %16.12f %16.12f %d\n", stops[i]->name,
               stops[i]->locale.latitude, stops[i]->locale.longitude,
               stops[i]->num_lines);
    }
}

/*
 * lists a single stop registered with the given name.
 * returns 0 if ok, -1 if the stop doesn't exit.
 */
int list_single_stop(char* name) {
    stop_t* stop;
    if (!(stop = get_stop(name)))
        return -1;
    printf("%16.12f %16.12f\n", stop->locale.latitude, stop->locale.longitude);
    return 0;
}

/*
 * registers a new stop.
 * returns 0 if all went well, -1 otherwise.
 */
int add_new_stop(const char* name, const double latitude,
                 const double longitude) {
    if (get_stop(name))
        return -1;

    if (!(stops[stop_counter] = (stop_t*)malloc(sizeof(stop_t)))) {
        printf("couldn't get memory for the new stop!\n");
        fprintf(stderr, "maybe this should panic instead\n");
        return 0;
    }

    strcpy(stops[stop_counter]->name, name);
    stops[stop_counter]->locale.latitude = latitude;
    stops[stop_counter]->locale.longitude = longitude;
    stops[stop_counter]->num_lines = 0;

    stop_counter++;
    return 0;
}

/*
 * p command.
 * receives a string which corresponds to the arguments of the command.
 * parsed with strtok (destructive).
 */
void list_or_add_stops(char* str) {
    char* token;
    char* name;

    strtok(str, "\"");
    if (!(name = strtok(NULL, "\""))) {
        if (!(name = strtok(str, DELIMITERS))) {
            list_all_stops();
            return;
        }
    }

    if (!(token = strtok(NULL, DELIMITERS))) {
        if (list_single_stop(name))
            printf("%s: no such stop.\n", name);
        return;
    }

    if (add_new_stop(name, atof(token), atof(strtok(NULL, DELIMITERS))))
        printf("%s: stop already exists.\n", name);
}

void get_c_input(const char* input, char* line_name, char* origin_name,
                 char* destination_name, double* cost, double* duration) {
    int ss_count = sscanf(input, "%s \"%[^\"]\" \"%[^\"]\" %lf %lf", line_name,
                          origin_name, destination_name, cost, duration);

    if (ss_count != 5)
        ss_count = sscanf(input, "%s \"%[^\"]\" %s %lf %lf", line_name,
                          origin_name, destination_name, cost, duration);
    if (ss_count != 5)
        ss_count = sscanf(input, "%s %s \"%[^\"]\" %lf %lf", line_name,
                          origin_name, destination_name, cost, duration);
    if (ss_count != 5)
        sscanf(input, "%s %s %s %lf %lf", line_name, origin_name,
               destination_name, cost, duration);
}
/*
 * l function.
 * adds a stop (or two in case they are the first) to a line.
 * receives a string with the arguments of the command.
 */
void add_connection(char* str) {
    char line_name[LINE_NAME_LENGTH + 1];
    char origin_name[STOP_NAME_LENGTH + 1];
    char destination_name[STOP_NAME_LENGTH + 1];
    double cost, duration;
    stop_t *origin, *destination;
    line_t* line;
    stop_node_t* tmp;

    get_c_input(str, line_name, origin_name, destination_name, &cost,
                &duration);

    if (!(line = get_line(line_name))) {
        printf("%s: no such line.\n", line_name);
        return;
    }

    if (!(origin = get_stop(origin_name))) {
        printf("%s: no such stop.\n", origin_name);
        return;
    }

    if (!(destination = get_stop(destination_name))) {
        printf("%s: no such stop.\n", destination_name);
        return;
    }

    if (cost < 0 || duration < 0) {
        printf("negative cost or duration.\n");
        return;
    }

    /*
     * not happy with the following code... at all!
     * but probably there is no other way of doing stupid code like this...
     */
    if (!line->origin && !line->destination) {
        line->origin = (stop_node_t*)malloc(sizeof(stop_node_t));
        line->destination = (stop_node_t*)malloc(sizeof(stop_node_t));
        line->origin->next = line->destination;
        line->origin->prev = NULL;
        line->destination->prev = line->origin;
        line->destination->next = NULL;

        line->origin->raw = origin;
        line->destination->raw = destination;

        line->total_cost += cost;
        line->total_duration += duration;
        line->num_stops = 2;
        origin->num_lines++;
        destination->num_lines++;
        return;
    }

    if (line->origin->raw != destination && line->destination->raw != origin) {
        printf("link cannot be associated with bus line.\n");
        return;
    }

    if (line->destination->raw == origin) {
        tmp = (stop_node_t*)malloc(sizeof(stop_node_t));
        tmp->prev = line->destination;
        tmp->next = NULL;
        tmp->raw = destination;
        line->destination->next = tmp;
        line->destination = tmp;
        if (line->origin->raw != destination)
            destination->num_lines++;
    } else {
        tmp = (stop_node_t*)malloc(sizeof(stop_node_t));
        tmp->next = line->origin;
        tmp->prev = NULL;
        tmp->raw = origin;
        line->origin->prev = tmp;
        line->origin = tmp;
        origin->num_lines++;
    }

    line->num_stops++;
    line->total_cost += cost;
    line->total_duration += duration;
}

/*
 * simple bubble sort for sorting a list of strings.
 */
void sort(char** list, int size) {
    int i, j;
    char* temp;
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (strcmp(list[j], list[j + 1]) > 0) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

/*
 * checks if a given line has the given stop.
 */
int intersects(const line_t* line, const stop_t* intersection) {
    stop_node_t* current = line->origin;
    while (current) {
        if (!strcmp(current->raw->name, intersection->name))
            return 1;
        current = current->next;
    }
    return 0;
}

/*
 * a single step of the i command.
 */
void print_intersction(const stop_t* intersection) {
    int i;
    int buffer_counter = 0;
    char** buffer = (char**)malloc(sizeof(char*) * line_counter);

    printf("%s %d:", intersection->name, intersection->num_lines);

    for (i = 0; i < line_counter; i++) {
        if (intersects(lines[i], intersection))
            buffer[buffer_counter++] = lines[i]->name;
    }
    sort(buffer, buffer_counter);

    for (i = 0; i < buffer_counter; i++)
        printf(" %s", buffer[i]);

    printf("\n");
    free(buffer);
}

/*
 * i command.
 * lists all the stops where lines intersect and those lines which intersect.
 */
void list_interconnections(char* str) {
    int i;
    /* there are no arguments to the i command */
    (void)str;

    for (i = 0; i < stop_counter; i++) {
        if (stops[i])
            if (stops[i]->num_lines > 1)
                print_intersction(stops[i]);
    }
}

int main(void) {
    lines = (line_t**)malloc(sizeof(line_t*) * MAX_LINES);
    stops = (stop_t**)malloc(sizeof(line_t*) * MAX_STOPS);
    char* buffer = (char*)malloc(sizeof(char) * BUFSIZ);
    char* buffer_offset = buffer;
    int exit = 0;

    buffer_offset++;
    while (!exit) {
        fgets(buffer, BUFSIZ, stdin);
        switch (*buffer) {
        case 'q':
            exit++;
            break;
        case 'c':
            list_or_add_line(buffer_offset);
            break;
        case 'p':
            list_or_add_stops(buffer_offset);
            break;
        case 'l':
            add_connection(buffer_offset);
            break;
        case 'i':
            list_interconnections(buffer);
            break;
        default:
            /* do nothing */
            break;
        }
    }
    free(buffer);
    return 0;
}
