#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    char language[50];
    double avg_time;
} BenchmarkResult;

typedef struct {
    char *command;
    double *result;
    char* lang;
} ThreadData;

double run_program(const char *command) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    system(command);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6; // Convert to milliseconds
    return time_taken;
}

void *benchmark_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    double total_time = 0.0;
    int runs = 5;
    for (int i = 0; i < runs; i++) {
        total_time += run_program(data->command);
    }

    *(data->result) = total_time / runs;
    return NULL;
}
typedef struct {
    char *key;
    char *value;
} KeyValue;

char* get_command(KeyValue dict[], int size, const char *key) {
    for (int i = 0; i < size; i++) {
        if (strcmp(dict[i].key, key) == 0) {
            return dict[i].value;
        }
    }
    return NULL;
}

void sort_results(BenchmarkResult results[], int size) {
    BenchmarkResult temp;
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (results[j].avg_time > results[j + 1].avg_time) {
                temp = results[j];
                results[j] = results[j + 1];
                results[j + 1] = temp;
            }
        }
    }
}

int main() {
    printf("Ditunggu sebentar (sekitar 1-2 menit)...\n\n\n");

    KeyValue commands[] = {
        {"C", "./bin/c"},
        {"C++", "./bin/cpp"},
        {"C#", "dotnet src/csharp/bin/Debug/net8.0/csharp.dll"},
        {"Pascal", "./bin/pascal/inverse"},
        {"Python3", "python3 src/inverse.py3"},
        {"Python2", "python2 src/inverse.py2"},
        {"PHP","php src/inverse.php"},
        {"Ruby","ruby src/inverse.rb"},
        {"Lua","lua src/inverse.lua"},
        {"Perl","perl src/inverse.pl"},
        {"Go","./bin/go"},
        {"Rust","./bin/rust"},
        {"Julia","julia src/inverse.jl"},
        {"Java","java -cp bin inverse"},
        {"R","Rscript src/inverse.R"},
        {"Swift","./bin/swift"},
        {"Scala","scala -cp bin/scala Inverse"},
        {"Dart","dart run src/inverse.dart"},
        {"F#","./bin/fsharp/fsharp"},
        {"Visual Basic","dotnet src/vbasic/bin/Debug/net8.0/vbasic.dll"},
        {"Java Script","node src/inverse.js"}
    };
    
    int num_languages = sizeof(commands) / sizeof(commands[0]);
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[num_cores];
    ThreadData thread_data[num_cores];
    BenchmarkResult results[num_languages];
    
    int idx = 0;
    while (idx < num_languages) {
       
        int num_threads = (num_languages - idx) < num_cores ? (num_languages - idx) : num_cores;

        for (int i = 0; i < num_threads; i++) {
            char *command = get_command(commands, num_languages, commands[idx + i].key);
            if (command) {
                thread_data[i].command = command;
                thread_data[i].result = &results[idx + i].avg_time;
                thread_data[i].lang = commands[idx+i].key;
                pthread_create(&threads[i], NULL, benchmark_thread, &thread_data[i]);
                strcpy(results[idx + i].language, commands[idx + i].key);
            }
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        
        idx += num_threads;
    }

    sort_results(results, num_languages);
    printf("Benchmark Results:\n");
    printf("----------------------------\n");
    for (int i = 0; i < num_languages; i++) {
        printf("%d. %s: %.2f ms\n", (i+1),results[i].language, results[i].avg_time);
    }
    printf("\n");
    return 0;
}
