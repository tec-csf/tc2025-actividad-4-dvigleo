/*
    Alumno: Daniela Vignau León
    Fecha: 10 de septiembre de 2020
    Materia: Programación Avanzada
    Profesor: Vicente Cubells
    Tarea 4
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>
#include <string.h>

#define H 15

typedef struct {
    int pid;
    int average;
    char * histogram;
} proc_t;

float get_average(int, int);

void create_histogram(proc_t *, int);
void print_table(proc_t *, int);

void print_help();
void free_memory(proc_t *, int);

int max = 0;

int main(int argc, char * const * argv) {
    char * input = NULL;
    int argument, start = 0, index;
    int n = 0;
    while((argument = getopt (argc, argv, "n:h")) != -1)
    
    switch(argument) {
        case 'n':
            input = optarg;
            if(isdigit(*input)) {
                n = atoi(input);
                start = 1;
            } else {
                fprintf(stderr, "Opción -%s require un número entero como argumento\n", optarg);
            }
            break;
        case 'h':
            print_help();
            break;
        case '?':
            if(optopt == 'n')
                fprintf(stderr, "Opción -%c require un número entero como argumento\n", optopt);
            else if(isprint (optopt))
                fprintf(stderr, "Opción desconocida '-%c\n", optopt);
            else
                fprintf(stderr, "Opción desconocida '\\x%x'\n", optopt);
            return 1;
        default:
            abort();
    }
    
    for (index = optind; index < argc; index++)
        printf ("El argumento %s no es una opción válida \n", argv[index]);
    
    if(start == 1) {
        pid_t pid;
        proc_t * processes = (proc_t *) malloc(sizeof(proc_t) * n);
        proc_t * p = processes;
        int average;
        float f_average;
        int i = 0;
        while(i < n) {
            pid = fork();
            if(pid == -1) {
                printf("Error. %d procesos hijos creados\n", i);
                exit(-1);
            } else if(pid == 0) { 
                f_average = get_average(getppid(), getpid());
                printf("Soy el proceso hijo con PID = %d, mi promedio es = %.2f\n", getpid(), f_average);
                average = (int)f_average;
                return f_average;
            } else {
                if (waitpid(pid, &average, 0) != -1) {
                    if (WIFEXITED(average)) {
                        (p+i)->pid = pid;
                        (p+i)->average = WEXITSTATUS(average);
                        
                        if(WEXITSTATUS(average) > max) 
                            max = WEXITSTATUS(average);
                        
                    }
                }
            }
            i++;
        }
        create_histogram(processes, n);
        print_table(processes, n);
        free_memory(processes, n);
    } 
    return 0;
}

float get_average(int ppid, int pid) {
    return (float)(ppid + pid) / 2;
}

void create_histogram(proc_t * processes, int n) {
    proc_t * p = processes;
    proc_t * final = processes + n;
    int n_chars = 0;
    char * c;
    for(; p < final; ++p) {
        n_chars = (p->average * H) / max;
        p->histogram = (char *) malloc(sizeof(char) * n_chars);
        strcpy(p->histogram, "*");
        c = p->histogram + 1;
        for(; c < p->histogram + n_chars; ++c) {
            strcpy(c, "*");
        }
    }
}

void print_table(proc_t * processes, int n) {
    proc_t * p = processes;
    proc_t * final = processes + n;
    printf("\nPID Hijo\tPromedio\tHistograma\n");
    for(; p < final; ++p) {
        printf("%d\t\t %d\t\t%s\n", p->pid, p->average, p->histogram);
    }
    printf("\n");
}

void print_help() {
    printf("\nUse: ./a.out [-n value] [-h]\n");
    printf("Opciones:\n");
    printf("\t-n : Crear n procesos indicados como valor entero\n\th : Ayuda\n");
}

void free_memory(proc_t * processes, int n) {
    proc_t * p = processes;
    proc_t * final = processes + n;
    for(; p < final; ++p) {
        p->pid = 0;
        p->average = 0;
        free(p->histogram);
    }    
    free(processes);
}