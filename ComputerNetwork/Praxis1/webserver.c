#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define BUFFER_SIZE 8192
#define MAX_RESOURCES 100
#define PATH_SIZE 256
int total_length = 0;
int bytes_received;
int yes = 1;
// List to store the request
int resource_number = 0;
char buffer[BUFFER_SIZE];
// create Structure to save the request
typedef struct _resource
{
    char path[PATH_SIZE];
    char content[PATH_SIZE];
} Resource; // Function to find the end of HTTP header (delimiter)
// Array of Resources
Resource resources[MAX_RESOURCES];
// Define HTTP responses
enum RESPONSES
{
    RESPONSE_GET,
    RESPONSE_FOO,
    RESPONSE_BAR,
    RESPONSE_BAZ,
    RESPONSE_FORBIDDEN,
    RESPONSE_NOCONTENT,
    RESPONSE_CONTENTCREATED,
    RESPONSE_DYNAMIC,
    RESPONSE_DYNAMIC_INVALID
};
const char *RESPONSE_400 =
    "HTTP/1.1 400 inkorrekte Anfragen\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char *RESPONSE_501 =
    "HTTP/1.1 501 alle anderen Anfragen\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char *RESPONSE_404_GET =
    "HTTP/1.1 404 GET-Anfragen\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char *RESPONSE_404_NOTFOUND = "HTTP/1.1 404 Not Found\r\n"
                                    "Content-Length: 0\r\n"
                                    "\r\n";

const char *RESPONSE_200_BAR = "HTTP/1.1 200 OK\r\n"
                               "Content-Length: 3\r\n"
                               "\r\n"
                               "Bar";

const char *RESPONSE_200_FOO = "HTTP/1.1 200 OK\r\n"
                               "Content-Length: 3\r\n"
                               "\r\n"
                               "Foo";

const char *RESPONSE_200_BAZ = "HTTP/1.1 200 OK\r\n"
                               "Content-Length: 3\r\n"
                               "\r\n"
                               "Baz";

const char *RESPONSE_204 =
    "HTTP/1.1 204 No Content\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

const char *RESPONSE_403 =
    "HTTP/1.1 403 Forbidden\r\n"
    "Content-Length: 0\r\n"
    "\r\n";

// search for /r/n/r/n position
int find_http_delimiter(const char *buffer, int length)
{
    for (int i = 0; i < length - 3; i++)
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n')
        {
            return i + 4;
        }
    }
    return -1;
}
// Find Resource in the Array. return -1 if the content is not found, otherwise return the position of the resource.
int find_resource(const char *path)
{
    // printf("finding resource...\n");
    for (int i = 0; i < resource_number; i++)
    {
        if (resources[i].path != NULL)
        {
            if (strcmp(resources[i].path, path) == 0)
            {
                return i; // Index of Ressource
            }
        }
    }
    return -1; // Resource not found
}

// This function is from LLM generated. The length of content in form of key and value should be extracted.
int extract_content_length(const char *buffer)
{
    const char *content_length_pos = strstr(buffer, "Content-Length:");
    if (!content_length_pos)
    {
        return -1; // Content-Length header not found
    }

    content_length_pos += strlen("Content-Length:"); // Move to the value
    while (*content_length_pos == ' ' || *content_length_pos == '\t')
    {
        content_length_pos++; // Skip spaces/tabs
    }

    // Ensure the value is numeric
    if (!isdigit(*content_length_pos))
    {
        return -1; // Invalid value
    }

    return atoi(content_length_pos); // Convert to integer
}

// Function to parse HTTP request
int parse_request(const char *buffer, int header, int client_fd)
{
    char method[32], uri[256], version[32];

    // Parse the request line
    if (sscanf(buffer, "%31s %255s %31s ", method, uri, version) != 3)
    {
        return 400; // Bad request if the start line is malformed
    }
    // Check https version
    if (strcmp(version, "HTTP/1.1") != 0 && strcmp(version, "HTTP/1.0") != 0)
    {
        return 400;
    }
    // Debug
    printf("Version is:%s\n", version);
    printf("Buffer is:\n%s\n", buffer);
    printf("Content is:\n%s\n", strstr(buffer, "\r\n\r\n") + 4);
    int resource_position = find_resource(uri);
    // Check if method is GET
    if (strcmp(method, "GET") == 0)
    {
        if (strcmp(uri, "/static/foo") == 0)
        {
            printf("Return FOO\n");
            return RESPONSE_FOO;
        }
        else if (strcmp(uri, "/static/bar") == 0)
        {
            printf("Return BAR\n");
            return RESPONSE_BAR;
        }
        else if (strcmp(uri, "/static/baz") == 0)
        {
            printf("Return BAZ\n");
            return RESPONSE_BAZ;
        }
        else if (strncmp(uri, "/dynamic/", 9) == 0)
        {
            if (resource_position != -1) // resouce is found
            {
                return RESPONSE_DYNAMIC;
            }
        }
        else
            return 404;
    }
    ////////////////////////////////////////////////////////////////
    else if (strcmp(method, "PUT") == 0)
    {
        printf("Handling PUT request...\n");

        if (strncmp(uri, "/dynamic/", 9) == 0)
        {
            int resource_position = find_resource(uri);
            // Locate the start of the content (payload)
            const char *content_start = strstr(buffer, "\r\n\r\n") + 4; // Skip past the header
            int content_length = extract_content_length(buffer);
            printf("Content-Length: %d\n", content_length);
            if (resource_position == -1)
            {
                // Resource not found, create a new one
                if (resource_number >= MAX_RESOURCES)
                {
                    printf("Too many resources, rejecting request.\n");
                    return RESPONSE_FORBIDDEN; // Too many resources
                }

                // Create new resource, idea of using strncpy is from LLM
                strncpy(resources[resource_number].path, uri, sizeof(resources[resource_number].path) - 1);
                resources[resource_number].path[sizeof(resources[resource_number].path) - 1] = '\0';

                strncpy(resources[resource_number].content, content_start, content_length);
                resources[resource_number].content[content_length] = '\0';
                resource_number++; // Increment the resource count

                return RESPONSE_CONTENTCREATED; // Successfully created new resource
            }
            else
            {
                // Resource exists, update it
                strncpy(resources[resource_position].content, content_start, content_length);
                resources[resource_position].content[content_length] = '\0';

                return RESPONSE_NOCONTENT; // Successfully updated existing resource
            }
        }
        else
        {
            printf("Waiting for more data...\n");
            return RESPONSE_FORBIDDEN; // Unauthorized or malformed request
        }
    }

    ////////////////////////////////////////////////////////////////
    else if (strcmp(method, "DELETE") == 0)
    {
        // Handle PUT request
        printf("Handling DELETE request...\n");
        // Check if path is /dynamic/
        if (strncmp(uri, "/dynamic/", 9) == 0)
        {
            printf("Dynamic path...\n");
            // Check if the file in resources
            int resource_position = find_resource(uri);
            if (resource_position != -1)
            {
                printf("Resource found");
                memset(resources[resource_position].content, 0, sizeof(resources[resource_position].content));
                memset(resources[resource_position].path, 0, sizeof(resources[resource_position].path));
                resource_number--;
                return RESPONSE_NOCONTENT;
            }
            else // if the file does not exist
            {
                printf("Resource not found");
                return RESPONSE_DYNAMIC_INVALID;
            }
        }
        else
            return RESPONSE_FORBIDDEN;
    }
    else
    {
        return 501; // Return 501 Not Implemented for other methods
    }
}

void handle_client(int client_fd)
{

    while (1)
    {
        bytes_received = recv(client_fd, buffer + total_length, BUFFER_SIZE - total_length, 0);
        if (bytes_received <= 0)
            break; // Connection closed or error
        total_length += bytes_received;
        buffer[total_length] = '\0'; // Null-terminate for safety
        char method[32], uri[256], version[32];
        int header_end;
        while ((header_end = find_http_delimiter(buffer, total_length)) > 0)
        {
            printf("Header ist : %d\n", header_end);
            // Log the received request
            printf("Received HTTP request:\n%.*s\n", header_end, buffer);
            int content_length = extract_content_length(buffer);
            const char *content_start = strstr(buffer, "\r\n\r\n") + 4;
            // Check if the payload has already arrived
            if (total_length < header_end + content_length)
            {
                // Try revieve packet again
                printf("Payload has not arrived yet...\n");
                break;
            }
            int status_code = parse_request(buffer, header_end, client_fd);
            switch (status_code)
            {
            case 400:
                send(client_fd, RESPONSE_400, strlen(RESPONSE_400), 0);
                break;

            case 501:
                send(client_fd, RESPONSE_501, strlen(RESPONSE_501), 0);
                break;

            case RESPONSE_FOO:
                send(client_fd, RESPONSE_200_FOO, strlen(RESPONSE_200_FOO), 0);
                break;

            case RESPONSE_BAR:
                send(client_fd, RESPONSE_200_BAR, strlen(RESPONSE_200_BAR), 0);
                break;

            case RESPONSE_BAZ:
                send(client_fd, RESPONSE_200_BAZ, strlen(RESPONSE_200_BAZ), 0);
                break;

            case RESPONSE_GET:
                send(client_fd, RESPONSE_404_GET, strlen(RESPONSE_404_GET), 0);
                break;

            case RESPONSE_NOCONTENT:
                send(client_fd, RESPONSE_204, strlen(RESPONSE_204), 0);
                break;

            case RESPONSE_FORBIDDEN:
                send(client_fd, RESPONSE_403, strlen(RESPONSE_403), 0);
                break;
            case RESPONSE_CONTENTCREATED:
            {
                if (sscanf(buffer, "%31s %255s %31s", method, uri, version) != 3)
                {
                    send(client_fd, RESPONSE_404_NOTFOUND, strlen(RESPONSE_404_NOTFOUND), 0);
                }
                else
                {
                    int resource_position = find_resource(uri);
                    printf("Content found at %s\n", uri);
                    char response_header[BUFFER_SIZE];
                    snprintf(response_header, sizeof(response_header),
                             "HTTP/1.1 201 Created\r\n"
                             "Content-Length: %d\r\n"
                             "\r\n"
                             "%s",
                             content_length, content_start);
                    send(client_fd, response_header, strlen(response_header), 0);
                }
                break;
            }

            case RESPONSE_DYNAMIC:
            {
                if (sscanf(buffer, "%31s %255s %31s", method, uri, version) != 3)
                {
                    send(client_fd, RESPONSE_404_NOTFOUND, strlen(RESPONSE_404_NOTFOUND), 0);
                }
                else
                {
                    int resource_position = find_resource(uri);
                    char response_header[BUFFER_SIZE];
                    snprintf(response_header, sizeof(response_header),
                             "HTTP/1.1 200 OK\r\n"
                             "Content-Length: %zu\r\n"
                             "\r\n"
                             "%s",
                             strlen(resources[resource_position].content), resources[resource_position].content);
                    send(client_fd, response_header, strlen(response_header), 0);
                }
                break;
            }
            default:
                send(client_fd, RESPONSE_404_NOTFOUND, strlen(RESPONSE_404_NOTFOUND), 0);
                break;
            }
            // Move any remaining data to the start of the buffer for the next iteration
            size_t remaining_data = BUFFER_SIZE - (header_end + content_length);
            if (remaining_data > 0)
            {
                memmove(buffer, buffer + header_end + content_length, remaining_data);
            }
            total_length -= (header_end + content_length);
        }
    }
    close(client_fd);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    char *port = argv[2];
    int sockfd;
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(ip, port, &hints, &servinfo) != 0)
    {
        perror("getaddrinfo");
        return 1;
    }

    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        close(sockfd);
        exit(1);
    }

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close(sockfd);
        perror("bind");
        return 1;
    }

    freeaddrinfo(servinfo);

    if (listen(sockfd, 10) == -1)
    {
        perror("listen");
        return 1;
    }

    printf("Server listening on %s:%s\n", ip, port);

    while (1)
    {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof client_addr;
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        handle_client(client_fd);
    }
    close(sockfd);
    return 0;
}
