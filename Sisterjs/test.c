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
        // Test GET with query parameters
        {"http://localhost:8080/nilai-akhir?name=Albert&nilai=90", "GET", NULL, NULL},
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

        // Test PUT with JSON content type
        {"http://localhost:8080/update", "PUT", "{\"name\": \"example3\", \"value\": 40}", "application/json"},
        // Test GET with query parameters
        {"http://localhost:8080/ambil-data", "GET", NULL, NULL},

        // Test DELETE request
        {"http://localhost:8080/delete", "DELETE", NULL, NULL},
        
        // Test GET without query parameters
        {"http://localhost:8080/nilai-akhir", "GET", NULL, NULL}
    };
    
    // Add an HTTP/2 specific request for testing
    Request http2_request = {"http://localhost:8080/nilai-akhir", "GET", NULL, NULL};

    int num_requests = sizeof(requests) / sizeof(Request);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    for (int i = 0; i < num_requests; i++) {
        curl = curl_easy_init();
        if(curl) {
            // Set the URL
            curl_easy_setopt(curl, CURLOPT_URL, requests[i].url);

            // Set the request method
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, requests[i].method);

            // If there is data, add it
            if (requests[i].data) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requests[i].data);
            }

            // Set the Content-Type header if necessary
            if (requests[i].content_type) {
                struct curl_slist *headers = NULL;
                char content_type_header[256];
                snprintf(content_type_header, sizeof(content_type_header), "Content-Type: %s", requests[i].content_type);
                headers = curl_slist_append(headers, content_type_header);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            }

            // Set the callback function to handle the response
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

            // Include headers in the output
            curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

            // Perform the request
            res = curl_easy_perform(curl);

            // Check for errors
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                // Print the full response
                printf("Response for %s %s:\n%s\n", requests[i].method, requests[i].url, response);
            }

            // Clean up
            curl_easy_cleanup(curl);

            // Clear the response buffer for the next request
            memset(response, 0, sizeof(response));
        }
    }

    // Now testing the HTTP/2 feature specifically
    curl = curl_easy_init();
    if(curl) {
        // Set the URL for HTTP/2 testing
        curl_easy_setopt(curl, CURLOPT_URL, http2_request.url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, http2_request.method);

        // Set HTTP/2 protocol
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        // Include headers in the output
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed for HTTP/2: %s\n", curl_easy_strerror(res));
        } else {
            // Print the full response
            printf("Response for %s %s (HTTP/2):\n%s\n", http2_request.method, http2_request.url, response);
        }

        // Clean up
        curl_easy_cleanup(curl);

        // Clear the response buffer for the next request
        memset(response, 0, sizeof(response));
    }

    curl_global_cleanup();
    return 0;
}
