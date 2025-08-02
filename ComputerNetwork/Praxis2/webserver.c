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
#include "util.h"
#include <pthread.h>
#include <fcntl.h> // For fcntl()

#define BUFFER_SIZE 8192
#define MAX_RESOURCES 100
#define PATH_SIZE 256
#define MAXBUFLEN 8192

int total_length = 0;
int bytes_received;
int yes = 1;
int total_length_UDP = 0;
int counter = 0;
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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Define HTTP responses
enum RESPONSES
{
    RESPONSE_303_dht,
    RESPONSE_303,
    RESPONSE_503_SERVICE_UNANVAILABLE,
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

struct env_variable
{
    char *pred_id;
    char *pred_ip;
    char *pred_port;
    char *succ_id;
    char *succ_ip;
    char *succ_port;
};

struct UDP_message
{
    uint8_t message_type;
    uint16_t hash_id;
    uint16_t node_id;
    uint32_t node_ip;
    uint16_t node_port;
};
struct thread_args
{
    int sock;                     // Socket descriptor (TCP or UDP)
    char *id;                     // Node ID
    char *ip;                     // IP address
    char *port;                   // Port number
    struct env_variable nodeInfo; // Node-specific environment variables
};
/*Saving Lookup answers*/
struct UDP_message answer[10];

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

/*search for /r/n/r/n position*/
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
void fill_udp_message(struct UDP_message *msg,
                      uint8_t message_type,
                      uint16_t hash_uri,
                      uint16_t node_id,
                      uint32_t node_ip,
                      uint16_t node_port)
{
    msg->message_type = message_type;
    msg->hash_id = hash_uri;
    msg->node_id = node_id;
    msg->node_ip = node_ip;
    msg->node_port = node_port;
}

void print_udp_messages(struct UDP_message answer[], int size)
{
    printf("\nMessage in List \n");
    for (int i = 0; i < size; i++)
    {
        printf("Message %d:\n", i + 1);
        printf("Message Type: %u\n", answer[i].message_type);
        printf("Hash ID: %u\n", answer[i].hash_id);
        printf("Node ID: %u\n", answer[i].node_id);

        // Convert the IP from integer to string format
        char ip_str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &answer[i].node_ip, ip_str, sizeof(ip_str)) != NULL)
        {
            printf("Node IP: %s\n", ip_str);
        }
        else
        {
            perror("inet_ntop");
        }

        printf("Node Port: %u\n\n", answer[i].node_port);
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}
void send_udp_lookup(char *ip, char *port, struct UDP_message *data)
{
    printf("In send_udp_lookup: IP = %s, Port = %s\n\n", ip, port);
    // Create the UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(strtol(port, NULL, 10));
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Serialize the UDP_message struct into a buffer

    uint8_t buffer_UDP[11];
    size_t offset = 0;

    buffer_UDP[offset++] = data->message_type;                 // 1 byte
    *(uint16_t *)(buffer_UDP + offset) = htons(data->hash_id); // 2 bytes
    offset += 2;
    *(uint16_t *)(buffer_UDP + offset) = htons(data->node_id); // 2 bytes
    offset += 2;
    *(uint32_t *)(buffer_UDP + offset) = htonl(data->node_ip); // 4 bytes
    offset += 4;
    *(uint16_t *)(buffer_UDP + offset) = htons(data->node_port); // 2 bytes
    offset += 2;

    for (size_t i = 0; i < sizeof(buffer_UDP); ++i)
    {
        printf("%02X ", buffer_UDP[i]);
    }
    printf("\n");

    // Send the message
    ssize_t sent = sendto(sockfd, buffer_UDP, sizeof(buffer_UDP), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent < 0)
    {
        perror("Failed to send UDP message");
    }
    else
    {
        printf("UDP message sent to %s:%s\n\n", ip, port);
    }
    // Close the socket
    close(sockfd);
}
int findResponsibleNode(struct UDP_message *answer, uint16_t requested_hash)
{
    printf("Finding responsible node....\n\n");
    printf("Requested Hash-ID: %d\n\n", requested_hash);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 10; i++)
    {
        printf("\nMessage-ID: %d\n", answer[i].message_type);
        printf("Hash-ID: %d\n", answer[i].hash_id);
        printf("Node-ID: %d\n\n", answer[i].node_id);

        if ((answer[i].message_type == 1 && requested_hash > answer[i].hash_id && requested_hash < answer[i].node_id) ||
            (answer[i].message_type == 1 && (answer[i].hash_id > answer[i].node_id) && (requested_hash > answer[i].hash_id || requested_hash < answer[i].node_id)))

        {
            /*Responsible node is found*/
            pthread_mutex_unlock(&mutex);
            return i;
        }
    }
    /*No responsible node is found*/
    pthread_mutex_unlock(&mutex);
    return -1;
}

int defineRequest(char *method, char *uri, int resource_position)
{
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
            else
                return 404;
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
        return 501;
}
// Function to parse HTTP request
int parse_request_TCP(const char *buffer, int header, int client_fd, char *id, char *port, uint16_t hashed_uri, struct env_variable nodeInfo)
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
    print_udp_messages(answer, 10);
    printf("Version is:%s\n", version);
    printf("Buffer is:\n%s\n", buffer);
    printf("Content is:\n%s\n", strstr(buffer, "\r\n\r\n") + 4);
    int resource_position = find_resource(uri);
    /*DHT*/
    uint16_t id_b = strtol(id, NULL, 10);
    printf("This node ID in Byte = %u\n", id_b);
    printf("This node Port  = %s\n", port);
    printf("Incoming request....\n\n");
    printf("Incoming ID = %u\n", hashed_uri);
    int responsibleNode = findResponsibleNode(answer, hashed_uri);

    printf("Responsible node = %d\n", responsibleNode);
    if (nodeInfo.succ_id != NULL && nodeInfo.succ_ip != NULL && nodeInfo.succ_port != NULL &&
        nodeInfo.pred_id != NULL && nodeInfo.pred_ip != NULL && nodeInfo.pred_port != NULL)
    {
        /*In range of succesor*/
        /*Start of DHT circle (Small ID) requested node Problem*/
        if ((strtol(nodeInfo.succ_id, NULL, 10) < strtol(id, NULL, 10) && ((hashed_uri >= strtol(id, NULL, 10)) || (hashed_uri < strtol(nodeInfo.succ_id, NULL, 10)))) ||
            (hashed_uri >= strtol(id, NULL, 10)) && (hashed_uri < strtol(nodeInfo.succ_id, NULL, 10)))
        {
            return RESPONSE_303;
        }
        else if (responsibleNode > -1)
        {
            return RESPONSE_303_dht;
        }
        /*In range of the node*/
        /*End of DHT circle (Big ID) Succesor node Problem */
        else if ((strtol(id, NULL, 10) < strtol(nodeInfo.pred_id, NULL, 10) && ((hashed_uri < strtol(id, NULL, 10)) || (hashed_uri > strtol(nodeInfo.pred_id, NULL, 10)))) ||
                 ((hashed_uri > strtol(nodeInfo.pred_id, NULL, 10)) && (hashed_uri <= strtol(id, NULL, 10))))
        {
            return defineRequest(method, uri, resource_position);
        }
        else
        {
            return RESPONSE_503_SERVICE_UNANVAILABLE;
        }
    }
    else
    {
        return defineRequest(method, uri, resource_position);
    }
}

void handle_client_TCP(int client_fd, char *id, char *ip, char *port, struct env_variable nodeInfo)
{
    while (1)
    {
        bytes_received = recv(client_fd, buffer + total_length, BUFFER_SIZE - total_length, 0);
        if (bytes_received <= 0)
            break; // Connection closed or error
        total_length += bytes_received;
        buffer[total_length] = '\0'; // Null-terminate for safety
        char method[32], uri[256], version[32];
        if (sscanf(buffer, "%31s %255s %31s", method, uri, version) != 3)
        {
            perror("Wrong Reqeust");
            send(client_fd, RESPONSE_404_NOTFOUND, strlen(RESPONSE_404_NOTFOUND), 0);
        }
        int header_end;
        uint16_t hashed_uri = pseudo_hash(uri, strlen(uri));
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
            int status_code = parse_request_TCP(buffer, header_end, client_fd, id, port, hashed_uri, nodeInfo);

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

                break;
            }

            case RESPONSE_DYNAMIC:
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

                break;
            }
            case RESPONSE_303:
            {
                char response_header[BUFFER_SIZE];
                snprintf(response_header, sizeof(response_header),
                         "HTTP/1.1 303 See Other\r\n"
                         "Location: http://%s:%s%s\r\n"
                         "Content-Length: 0\r\n"
                         "\r\n",
                         nodeInfo.succ_ip, nodeInfo.succ_port, uri);
                send(client_fd, response_header, strlen(response_header), 0);
                break;
            }
            case RESPONSE_303_dht:
            {
                int responsibleNode = findResponsibleNode(answer, hashed_uri);

                int answer_host = htonl(answer[responsibleNode].node_ip);
                char ip_str[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &answer_host, ip_str, sizeof(ip_str)) != NULL)
                {
                    printf("Node IP: %s\n", ip_str);
                }
                else
                {
                    perror("inet_ntop");
                }

                /*Convert Port Byte to String*/
                char port_str[16]; // Buffer to hold the string representation
                snprintf(port_str, sizeof(port_str), "%d", answer[responsibleNode].node_port);

                char response_header[BUFFER_SIZE];
                snprintf(response_header, sizeof(response_header),
                         "HTTP/1.1 303 See Other\r\n"
                         "Location: http://%s:%s%s\r\n"
                         "Content-Length: 0\r\n"
                         "\r\n",
                         ip_str, port_str, uri);

                send(client_fd, response_header, strlen(response_header), 0);
                break;
            }
            case RESPONSE_503_SERVICE_UNANVAILABLE:
            {
                char response_header[BUFFER_SIZE];
                snprintf(response_header, sizeof(response_header),
                         "HTTP/1.1 503 Service Unavailable\r\n"
                         "Retry-After: 1\r\n"
                         "Content-Length: 0\r\n"
                         "\r\n");

                struct UDP_message message;

                uint32_t succ_ip_b;
                if (inet_pton(AF_INET, nodeInfo.succ_ip, &succ_ip_b) != 1)
                {
                    fprintf(stderr, "Invalid IP address: %s\n", nodeInfo.succ_ip);
                    exit(EXIT_FAILURE);
                }

                printf("Hashed URI: %u\n", hashed_uri);
                fill_udp_message(&message, strtol("0", NULL, 10), hashed_uri, (strtol(id, NULL, 10)), ntohl(succ_ip_b), (strtol(port, NULL, 10)));

                printf("Sending UDP message from the first node...\n");
                printf("Message Type: %u\n", message.message_type);
                printf("Hash ID: %u\n", message.hash_id);
                printf("Node ID: %u\n", message.node_id);
                printf("Node IP: %u\n", message.node_ip);
                printf("Node Port: %u\n", message.node_port);
                /*Send Look Up to succesor*/
                send(client_fd, response_header, strlen(response_header), 0);
                send_udp_lookup(nodeInfo.succ_ip, nodeInfo.succ_port, &message);

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

/*****************************************************************************************************************/
// Function to handle UDP communication
void handle_client_UDP(int udp_socket, char *id, char *ip, char *port, struct env_variable nodeInfo)
{
    struct sockaddr_storage client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char recv_buffer[MAXBUFLEN];

    while (1)
    {

        memset(recv_buffer, 0, MAXBUFLEN); // Clear the buffer
        int byte_recieved_UDP = recvfrom(udp_socket, recv_buffer, MAXBUFLEN - 1, 0,
                                         (struct sockaddr *)&client_addr, &addr_len);

        printf("Receiving UDP Message: %d\n", byte_recieved_UDP);
        if (byte_recieved_UDP < 0)
        {
            perror("recvfrom");
            return;
        }

        /*Network to host decoding*/
        struct UDP_message message;
        struct UDP_message received;
        size_t offset = 0;

        received.message_type = recv_buffer[offset++];
        received.hash_id = ntohs(*(uint16_t *)(recv_buffer + offset));
        offset += 2;
        received.node_id = ntohs(*(uint16_t *)(recv_buffer + offset));
        offset += 2;
        received.node_ip = ntohl(*(uint32_t *)(recv_buffer + offset));
        offset += 4;
        received.node_port = ntohs(*(uint16_t *)(recv_buffer + offset));
        offset += 2;
        recv_buffer[byte_recieved_UDP] = '\0';

        pthread_mutex_lock(&mutex);
        /*Saving incoming answer in array with length of 10*/
        if (counter >= 10)
        {
            /*Overwrite the first element of Array by moving the rest of the array to the start position*/
            memmove(answer, answer + 1, 9 * sizeof(struct UDP_message));
            counter = 9;
        }

        answer[counter++] = received;

        pthread_mutex_unlock(&mutex);

        printf("Done saving answer\n");

        if (received.message_type == 1)
        {
            printf("End of lookup\n");
        }
        // (received.hash_id >= strtol(nodeInfo.pred_id, NULL, 10)) && (received.hash_id < strtol(id, NULL, 10))
        /*In range of succesor, reply back to the first node*/
        else if (((received.hash_id >= strtol(id, NULL, 10)) && (received.hash_id < strtol(nodeInfo.succ_id, NULL, 10))) ||
                 ((strtol(nodeInfo.succ_id, NULL, 10) < strtol(id, NULL, 10)) && ((received.hash_id >= strtol(id, NULL, 10)) || (received.hash_id < strtol(nodeInfo.succ_id, NULL, 10)))))
        {
            uint32_t succ_ip_b;
            if (inet_pton(AF_INET, nodeInfo.succ_ip, &succ_ip_b) != 1)
            {
                fprintf(stderr, "Invalid IP address: %s\n", nodeInfo.succ_ip);
                exit(EXIT_FAILURE);
            }

            /*Response with the node that is responsible for the request*/
            fill_udp_message(&message, strtol("1", NULL, 10), strtol(id, NULL, 10), strtol(nodeInfo.succ_id, NULL, 10), ntohl(succ_ip_b), strtol(nodeInfo.succ_port, NULL, 10));

            printf("Sending responsible node with UDP...\n");
            printf("Message Type: %u\n", message.message_type);
            printf("Hash ID: %u\n", message.hash_id);
            printf("Node ID: %u\n", message.node_id);
            printf("Node IP: %u\n", message.node_ip);
            printf("Node Port: %u\n", message.node_port);

            /*Host byte-order to Network byte-order*/
            int ip_Host = htonl(received.node_ip);
            /*Convert IP Byte to String*/
            char ip_str[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &(ip_Host), ip_str, sizeof(ip_str)) != NULL)
            {
                printf("Node IP: %s\n", ip_str);
            }
            else
            {
                perror("inet_ntop");
            }
            /*Convert Port Byte to String*/
            char port_str[16]; // Buffer to hold the string representation
            snprintf(port_str, sizeof(port_str), "%d", received.node_port);

            send_udp_lookup(ip_str, port_str, &message);
            printf("Responsible node is sent...\n");
        }
        /*Pass the request forward to the successcor*/
        else
        {
            /*Forwarding the request to the succescor*/
            fill_udp_message(&message, strtol("0", NULL, 10), received.hash_id, received.node_id, received.node_ip, received.node_port);

            printf("Forwarding UDP Message...\n");
            printf("Message Type: %u\n", message.message_type);
            printf("Hash ID: %u\n", message.hash_id);
            printf("Node ID: %u\n", message.node_id);
            printf("Node IP: %u\n", message.node_ip);
            printf("Node Port: %u\n", message.node_port);

            send_udp_lookup(nodeInfo.succ_ip, nodeInfo.succ_port, &message);
        }
    }
}

static int setup_server_socket(char *ip, char *port, struct addrinfo hints, struct addrinfo *servinfo, int protocol)
{
    struct addrinfo *p;
    int sockfd;
    if (getaddrinfo(ip, port, &hints, &servinfo) != 0)
    {
        perror("getaddrinfo");
        return 1;
    }
    /*Keep trying to establish connection*/
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        {
            perror("setsockopt SO_REUSEADDR failed for UDP socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    if (protocol == SOCK_STREAM)
    {
        if (listen(sockfd, 10) == -1)
        {
            perror("listen");
            return 1;
        }
    }

    printf("Server listening on %s:%s\n", ip, port);

    return sockfd;
}
/*LLM generated*/
void *udp_thread_func(void *arg)
{
    struct thread_args *udp_args = (struct thread_args *)arg;
    handle_client_UDP(udp_args->sock, udp_args->id, udp_args->ip, udp_args->port, udp_args->nodeInfo);

    return NULL;
}
/*LLM generated*/
void *tcp_thread_func(void *arg)
{
    struct thread_args *tcp_args = (struct thread_args *)arg;
    handle_client_TCP(tcp_args->sock, tcp_args->id, tcp_args->ip, tcp_args->port, tcp_args->nodeInfo);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        return 1;
    }
    char *id;
    char *ip = argv[1];
    char *port = argv[2];
    if (argv[3] == NULL)
        id = "0";
    else
        id = argv[3];

    int sock_TCP, sock_UDP;
    struct addrinfo hints_TCP, hints_UDP, *servinfo;
    struct env_variable nodeInfo =
        {
            .pred_id = getenv("PRED_ID"),
            .pred_ip = getenv("PRED_IP"),
            .pred_port = getenv("PRED_PORT"),
            .succ_id = getenv("SUCC_ID"),
            .succ_ip = getenv("SUCC_IP"),
            .succ_port = getenv("SUCC_PORT")};

    printf("nodeInfo:\n\n");
    printf("pred_id: %s\n", nodeInfo.pred_id ? nodeInfo.pred_id : "NULL");
    printf("pred_ip: %s\n", nodeInfo.pred_ip ? nodeInfo.pred_ip : "NULL");
    printf("pred_port: %s\n", nodeInfo.pred_port ? nodeInfo.pred_port : "NULL");
    printf("succ_id: %s\n", nodeInfo.succ_id ? nodeInfo.succ_id : "NULL");
    printf("succ_ip: %s\n", nodeInfo.succ_ip ? nodeInfo.succ_ip : "NULL");
    printf("succ_port: %s\n", nodeInfo.succ_port ? nodeInfo.succ_port : "NULL");

    /* TCP Socket */
    memset(&hints_TCP, 0, sizeof(hints_TCP));
    hints_TCP.ai_family = AF_UNSPEC;
    hints_TCP.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints_TCP.ai_flags = AI_PASSIVE;

    /* UDP Socket */
    memset(&hints_UDP, 0, sizeof(hints_UDP));
    hints_UDP.ai_family = AF_UNSPEC;
    hints_UDP.ai_socktype = SOCK_DGRAM; // UDP stream sockets
    hints_UDP.ai_flags = AI_PASSIVE;

    /* Bind sockets */
    sock_TCP = setup_server_socket(ip, port, hints_TCP, servinfo, SOCK_STREAM);
    sock_UDP = setup_server_socket(ip, port, hints_UDP, servinfo, SOCK_DGRAM);

    printf("Server listening on %s:%s (TCP and UDP)\n", ip, port);

    // Create threads for UDP and TCP
    pthread_t udp_thread, tcp_thread;

    // Arguments for UDP thread
    struct thread_args udp_args = {sock_UDP, id, ip, port, nodeInfo};
    if (pthread_create(&udp_thread, NULL, udp_thread_func, &udp_args) != 0)
    {
        perror("Error creating UDP thread");
        return 1;
    }

    // Handle incoming TCP connections in the main thread
    while (1)
    {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_fd = accept(sock_TCP, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd == -1)
        {
            perror("accept");
            continue;
        }

        // Create a new thread for each TCP client
        struct thread_args *tcp_args = malloc(sizeof(struct thread_args));
        if (!tcp_args)
        {
            perror("malloc");
            close(client_fd);
            continue;
        }

        tcp_args->sock = client_fd;
        tcp_args->id = id;
        tcp_args->ip = ip;
        tcp_args->port = port;
        tcp_args->nodeInfo = nodeInfo;

        if (pthread_create(&tcp_thread, NULL, tcp_thread_func, tcp_args) != 0)
        {
            perror("Error creating TCP thread");
            close(client_fd);
            free(tcp_args);
            continue;
        }

        // Detach the thread to reclaim resources when it exits
        // pthread_detach(tcp_thread);
    }

    // Clean up
    close(sock_UDP);
    close(sock_TCP);
    pthread_join(tcp_thread, NULL);
    pthread_join(udp_thread, NULL);
    pthread_mutex_destroy(&mutex);

    return 0;
}
