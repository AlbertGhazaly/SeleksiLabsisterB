#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

// Function to write the response to a string
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    strncat((char *)userp, (char *)contents, realsize);
    return realsize;
}

// Struct to hold request details
typedef struct {
    char *url;
    char *method;
    char *data;
    char *content_type;
} Request;

int main(void) {
    CURL *curl;
    CURLcode res;
    char response[2048] = {0};  // Buffer to store the response

    // List of requests to test the server's features
    Request requests[] = {
        // Test GET without query parameters
        {"http://localhost:8080/nilai-akhir", "GET", NULL, NULL},
        // Test GET with query parameters
        {"http://localhost:8080/nilai-akhir?name=Albert&nilai=90", "GET", NULL, NULL},

        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test POST with text/plain
        {"http://localhost:8080/submit", "POST", "{\"name\": \"example\", \"value\": 43}", "text/plain"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test POST with application/json
        {"http://localhost:8080/submit", "POST", "{\"name\": \"example\", \"value\": 42}", "application/json"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test POST with application/x-www-form-urlencoded
        {"http://localhost:8080/submit", "POST", "name=example2&value=41", "application/x-www-form-urlencoded"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test PUT with text/plain
        {"http://localhost:8080/update", "PUT", "{\"name\": \"example3\", \"value\": 40}", "text/plain"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test PUT with JSON content type
        {"http://localhost:8080/update", "PUT", "{\"name\": \"example3\", \"value\": 39}", "application/json"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test PUT with application/x-www-form-urlencoded
        {"http://localhost:8080/update", "PUT", "{\"name\": \"example3\", \"value\": 38}", "application/x-www-form-urlencoded"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},


        // Test DELETE request
        {"http://localhost:8080/delete", "DELETE", NULL, NULL}
        
       
    };
    
    
    int num_requests = sizeof(requests) / sizeof(Request);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    for (int i = 0; i < num_requests; i++) {
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, requests[i].url);

            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, requests[i].method);

            if (requests[i].data) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requests[i].data);
            }

            if (requests[i].content_type) {
                struct curl_slist *headers = NULL;
                char content_type_header[256];
                snprintf(content_type_header, sizeof(content_type_header), "Content-Type: %s", requests[i].content_type);
                headers = curl_slist_append(headers, content_type_header);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

            curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                printf("Response for %s %s:\n%s\n", requests[i].method, requests[i].url, response);
            }

            curl_easy_cleanup(curl);

            memset(response, 0, sizeof(response));
        }
    }

    // Testing Http2
    Request http2_request = {"http://localhost:8080/nilai-akhir", "GET", NULL, NULL};

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, http2_request.url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, http2_request.method);

        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed for HTTP/2: %s\n", curl_easy_strerror(res));
        } else {
            printf("Response for %s %s (HTTP/2):\n%s\n", http2_request.method, http2_request.url, response);
        }

        curl_easy_cleanup(curl);

        memset(response, 0, sizeof(response));
    }

    curl_global_cleanup();
    return 0;
}
