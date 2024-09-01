#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "hash.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_EVENTS 10

typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} ClientData;

// Function prototypes
void handle_http2_request(int client_socket);
void handle_request_thread_based(int client_socket);
void handle_request_event_based(int client_socket);
void *thread_handler(void *arg);
void start_thread_based_server();
void start_event_based_server();
void route_request(int client_socket, const char *method, const char *path, const char *body, HashTable *query_params, char* content_type);
void parse_query_string(char *query_string, HashTable *query_params);
char *get_query_param(HashTable *query_params, const char *key);
HashTable *globalTable;

int calclen(char * a){
    int i = 0;
    for (;*(a+i)!='\0';i++){}
    return i;
}

// Parser
void parse_query_string(char *path, HashTable *query) {
        char *query_start = path;
        char *token = strtok(query_start, "&");
        int space = 0;
        while (token) {
            space += calclen(token);
            printf("token: %s\n",token);
            char *key = strtok(token, "=");
            char *value = strtok(NULL, "=");
            if (key && value) {
                insert(query, key, value);
            }
            space++;
            token = strtok(query_start+space, "&");
        }
}

char *get_query_param(HashTable *query_params, const char *key) {
    return get(query_params, key);
}

void parse_urlencoded(const char *body, HashTable *hashtable) {
    char *body_copy = strdup(body); 
    char *pair = strtok(body_copy, "&");

    while (pair) {
        char *equal_sign = strchr(pair, '=');
        if (equal_sign) {
            *equal_sign = '\0';
            char *key = pair;
            char *value = equal_sign + 1;

            if (key && value) {
                insert(hashtable, key, value); 
            }
        }
        pair = strtok(NULL, "&"); 
    }

    free(body_copy); 
}


void parse_json(const char *body, HashTable *hashtable) {
    char *body_copy = strdup(body);
    char *key = strtok(body_copy, ":{},\" \n");
    char *value;

    while (key != NULL) {
        value = strtok(NULL, ":{},\" \n");
        if (key && value) {
            insert(hashtable, key, value);
        }
        key = strtok(NULL, ":{},\" \n");
    }

    free(body_copy);
}

void parse_plain_text(const char *body, HashTable *hashtable) {
    char *line, *key, *value;
    char *body_copy = strdup(body);
    char *saveptr;

    line = strtok_r(body_copy, "\n", &saveptr);
    while (line != NULL) {
        key = strtok(line, ":");
        value = strtok(NULL, "");

        if (key && value) {
            char *key_trimmed = strtok(key, " \t");
            char *value_trimmed = strtok(value, " \t");

            insert(hashtable, key_trimmed, value_trimmed);
        }

        line = strtok_r(NULL, "\n", &saveptr);
    }

    free(body_copy);
}

char *stringify_json(HashTable *hashtable) {
    char *json = malloc(1024); 
    if (json == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strcpy(json, "{"); 
    int first_pair = 1; 

    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyValue *pair = hashtable->table[i];
        while (pair != NULL) {
            if (!first_pair) {
                strcat(json, ", "); 
            }
            strcat(json, "\"");
            strcat(json, pair->key);
            strcat(json, "\": \"");
            strcat(json, pair->value);
            strcat(json, "\"");
            first_pair = 0;
            pair = pair->next; 
        }
    }

    strcat(json, "}"); 
    return json; 
}

// Method handler
void handle_get(int client_socket, const char *path, HashTable *query_params) {
    if (strcmp(path, "/nilai-akhir") == 0) {
        const char *name = get_query_param(query_params, "name");
        const char *nilai = get_query_param(query_params, "nilai");
        char response[BUFFER_SIZE];
        if (name) {
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello %s, Nilai Akhir kamu %s\n", name,nilai);
        } else {
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nNilai Akhir kamu 50 (kkm) karena tidak pakai query\n");
        }
        send(client_socket, response, strlen(response), 0);
    }else if (strcmp(path, "/ambil-data") == 0){
        const char *data = stringify_json(globalTable);
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nData nilai: %s\n",data);
        send(client_socket,response,strlen(response),0);
    } else {
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found\n";
        send(client_socket, response, strlen(response), 0);
    }
}

void handle_post(int client_socket, const char *path, const char *body, char* content_type) {
    if (strcmp(path, "/submit") == 0) {
        printf("name ori: %s\n",body);

        HashTable *body_params = malloc(sizeof(HashTable));
        memset(body_params->table, 0, sizeof(KeyValue *) * TABLE_SIZE);
        if (strcmp(content_type, "application/x-www-form-urlencoded")==0) {
            parse_urlencoded(body, body_params);
         
        } else if (strcmp(content_type, "application/json")==0) {
            parse_json(body, body_params);
           
        } else if (strcmp(content_type, "text/plain")==0) {
            parse_plain_text(body,body_params);
        }

        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST: Data Submitted\n";
        send(client_socket, response, strlen(response), 0);
        printf("name body: %s\n",get(body_params,"name"));
        printf("value body: %s\n",get(body_params,"value"));
        
        merge_tables(globalTable,body_params);
        printf("name: %s\n",get(globalTable,"name"));
        free_table(body_params);
        free(body_params);

    } else {
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found\n";
        send(client_socket, response, strlen(response), 0);
    }
}

void handle_put(int client_socket, const char *path, const char *body,char* content_type) {
    if (strcmp(path, "/update") == 0) {
        HashTable *body_params = malloc(sizeof(HashTable));
        memset(body_params->table, 0, sizeof(KeyValue *) * TABLE_SIZE);
        if (strcmp(content_type, "application/x-www-form-urlencoded")==0) {
            parse_urlencoded(body, body_params);
         
        } else if (strcmp(content_type, "application/json")==0) {
            parse_json(body, body_params);
           
        } else if (strcmp(content_type, "text/plain")==0) {
            parse_plain_text(body,body_params);
        }
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPUT: Data Updated\n";
        send(client_socket, response, strlen(response), 0);

        merge_tables(globalTable,body_params);

        free_table(body_params);
        free(body_params);
    } else {
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found\n";
        send(client_socket, response, strlen(response), 0);
    }
}

void handle_delete(int client_socket, const char *path) {
    if (strcmp(path, "/delete") == 0) {
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nDELETE: Data Deleted\n";
        send(client_socket, response, strlen(response), 0);
    } else {
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found\n";
        send(client_socket, response, strlen(response), 0);
    }
}

// Routing
void route_request(int client_socket, const char *method, const char *path, const char *body, HashTable *query_params, char* content_type) {
    if (strcmp(method, "GET") == 0) {
        handle_get(client_socket, path, query_params);
    } else if (strcmp(method, "POST") == 0) {
        handle_post(client_socket, path, body,content_type);
    } else if (strcmp(method, "PUT") == 0) {
        handle_put(client_socket, path, body,content_type);
    } else if (strcmp(method, "DELETE") == 0) {
        handle_delete(client_socket, path);
    } else {
        const char *response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMethod Not Allowed\n";
        send(client_socket, response, strlen(response), 0);
    }
}
// HTTP/2 requests handler
void handle_http2_request(int client_socket) {
    const char *response = "HTTP/2 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from HTTP/2!\n";
    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}

// Thread handler 
void *thread_handler(void *arg) {
    ClientData *client_data = (ClientData *)arg;
    handle_request_thread_based(client_data->client_socket);
    free(client_data);
    return NULL;
}


// thread request handler
void handle_request_thread_based(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        char method[8], path[256], body[BUFFER_SIZE], query_string[256];
        query_string[0] = '\0';
        char content_type[256];

        sscanf(buffer, "%s %s", method, path);
        char *body_ptr = strstr(buffer, "\r\n\r\n");
        if (body_ptr) {
            strcpy(body, body_ptr + 4);
        } else {
            body[0] = '\0';
        }

        char *query_ptr = strchr(path, '?');
        if (query_ptr) {
            strcpy(query_string, query_ptr + 1);
            *query_ptr = '\0';
        }

        HashTable query_params;
        memset(&query_params, 0, sizeof(HashTable));

        if (strlen(query_string) > 0) {
            parse_query_string(query_string, &query_params);
        }
        char *header_ptr = strstr(buffer, "\r\n") + 2; 
        char *line_end;


        content_type[0] = '\0';
        
        while ((line_end = strstr(header_ptr, "\r\n")) != NULL && header_ptr < body_ptr) {
            *line_end = '\0';
            if (strncasecmp(header_ptr, "Content-Type:", 13) == 0) {
                strcpy(content_type, header_ptr + 14);
                char *newline_pos = strchr(content_type, '\r');
                if (newline_pos) {
                    *newline_pos = '\0';
                }
            }
            header_ptr = line_end + 2; 
        }

       
        // check http type
        if (strstr(buffer, "Upgrade: h2c") != NULL) {
            handle_http2_request(client_socket);
        } else {
            route_request(client_socket, method, path, body, &query_params,content_type);
        }

        free_table(&query_params);
    }

    close(client_socket);
}

// event request handler
void handle_request_event_based(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; 
        char method[8], path[256], body[BUFFER_SIZE], query_string[256];
        query_string[0] = '\0';
        char content_type[255];


        sscanf(buffer, "%s %s", method, path);
        char *body_ptr = strstr(buffer, "\r\n\r\n");
        if (body_ptr) {
            strcpy(body, body_ptr + 4);
        } else {
            body[0] = '\0';
        }

        char *query_ptr = strchr(path, '?');
        if (query_ptr) {
            strcpy(query_string, query_ptr + 1);
            *query_ptr = '\0';
        }

        HashTable query_params;
        memset(&query_params, 0, sizeof(HashTable));

        if (strlen(query_string) > 0) {
            parse_query_string(query_string, &query_params);
        }

        char *header_ptr = strstr(buffer, "\r\n") + 2; 
        char *line_end;
        content_type[0] = '\0';
        
        while ((line_end = strstr(header_ptr, "\r\n")) != NULL && header_ptr < body_ptr) {
            *line_end = '\0'; 
            if (strncasecmp(header_ptr, "Content-Type:", 13) == 0) {
                strcpy(content_type, header_ptr + 14); 
                char *newline_pos = strchr(content_type, '\r');
                if (newline_pos) {
                    *newline_pos = '\0'; 
                }
            }
            header_ptr = line_end + 2; 
        }
        // check http type
        if (strstr(buffer, "Upgrade: h2c") != NULL) {
            handle_http2_request(client_socket);
        } else {
            route_request(client_socket, method, path, body, &query_params,content_type);
        }

        free_table(&query_params);
    }

    close(client_socket);
}

// thread server
void start_thread_based_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);

    printf("Thread-based server started on port %d\n", PORT);

    while (1) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        ClientData *client_data = (ClientData *)malloc(sizeof(ClientData));
        client_data->client_socket = client_socket;
        client_data->client_addr = client_addr;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, thread_handler, client_data);
        pthread_detach(thread_id);
    }

    close(server_socket);
}

// event server
void start_event_based_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);

    printf("Event-based server started on port %d\n", PORT);

    int epoll_fd = epoll_create1(0);
    struct epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event);

    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_socket) {
                int client_socket;
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

                int flags = fcntl(client_socket, F_GETFL, 0);
                fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

                event.data.fd = client_socket;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);
            } else {
                handle_request_event_based(events[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }

    close(server_socket);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <thread|event>\n", argv[0]);
        return 1;
    }
    globalTable = malloc(sizeof(HashTable));
    memset(globalTable->table, 0, sizeof(KeyValue *) * TABLE_SIZE);

    if (strcmp(argv[1], "thread") == 0) {
        start_thread_based_server();
    } else if (strcmp(argv[1], "event") == 0) {
        start_event_based_server();
    } else {
        printf("Invalid argument. Use 'thread' or 'event'.\n");
        free_table(globalTable);
        free(globalTable);
        return 1;
    }
    free_table(globalTable);
    free(globalTable);
    return 0;
}
