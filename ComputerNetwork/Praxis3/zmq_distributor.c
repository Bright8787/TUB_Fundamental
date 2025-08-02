#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#define msg_len 1500 // Adjust chunk size as needed

struct thread_args
{
    int worker_port;
    char **buffer_chunk;
    int number_of_chunks;
    char *result;
};
struct hashElement
{
    char *key;
    int frequency;
    struct hashElement *next;
};

void toLowerCase(char *str)
{
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}

struct hashElement *createNode(char *key, int frequency, int key_len)
{
    struct hashElement *newNode = (struct hashElement *)malloc(sizeof(struct hashElement)); // Freed in FreeList
    if (newNode == NULL)
    {
        printf("Failed to allocate");
        return NULL;
    }
    newNode->key = strdup(key); // Freed in FreeList
    if (newNode->key == NULL)
    {
        printf("Failed to allocate Key");
        return NULL;
    }
    newNode->frequency = frequency;
    newNode->next = NULL;
    return newNode;
}

/*LLM generated the structure, implementation is from me*/
void insert_or_update(struct hashElement **head, char *new_key, int frequency, int key_len)
{
    // printf("In insert_or_update\n");
    // Case 1: If the list is empty, insert the first node
    if (*head == NULL)
    {
        struct hashElement *newNode = createNode(new_key, frequency, key_len); // Creating the first node
        *head = newNode;                                                       // Set the head to point to the new node
        // printf("Inserting new Node: %s\n", new_key);
        return;
    }

    // Case 2: Traverse the list to check if the key exists and update its frequency
    struct hashElement *temp = *head; // Start from the head of the list
    while (temp != NULL)
    {
        if (strcmp(temp->key, new_key) == 0) // Compare the full key using strcmp
        {
            // printf("Key already exists. Updating frequency\n");
            // printf("New Key frequency is:%d\n", frequency + temp->frequency);
            temp->frequency = temp->frequency + frequency; // Update the frequency
            return;                                        // Exit after updating
        }
        temp = temp->next; // Move to the next node
    }

    // Case 3: If the key does not exist, insert a new node at the end of the list
    // printf("Inserting new Node at the end: %s\n", new_key);
    struct hashElement *newNode = createNode(new_key, frequency, key_len); // Create a new node with frequency 1
    temp = *head;                                                          // Start from the head again
    while (temp->next != NULL)
    {
        temp = temp->next; // Traverse until the end
    }
    temp->next = newNode; // Insert the new node at the end
    return;
}
void freeList(struct hashElement **head_ref)
{
    struct hashElement *current = *head_ref;
    struct hashElement *next;

    while (current != NULL)
    {
        next = current->next;
        // printf("Key: %s ", current->key);
        free(current->key); // Free allocated word
        free(current);      // Free the current node
        current = next;
    }

    *head_ref = NULL; // Set head to NULL after freeing
    return;
}
void printList_frequency(struct hashElement *head, char *msg, int total_size)
{
    struct hashElement *temp = head;
    while (temp != NULL)
    {
        printf("%s,%d\n", temp->key, temp->frequency);
        temp = temp->next;
    }
    return;
}

/*LLM generated */
void sortList(struct hashElement **head)
{
    if (*head == NULL || (*head)->next == NULL)
    {
        return; // No need to sort if the list is empty or has one element
    }

    int swapped;
    struct hashElement *ptr1;
    struct hashElement *lptr = NULL; // Tracks the last sorted hashElement

    do
    {
        swapped = 0;
        ptr1 = *head;

        while (ptr1->next != lptr)
        {
            struct hashElement *ptr2 = ptr1->next;

            // Compare based on frequency, then alphabetically
            if ((ptr1->frequency < ptr2->frequency) ||
                (ptr1->frequency == ptr2->frequency && strcmp(ptr1->key, ptr2->key) > 0))
            {
                // Swap keys by swapping pointers (safer)
                char *tempKey = ptr1->key;
                ptr1->key = ptr2->key;
                ptr2->key = tempKey;

                // Swap frequency
                int tempFreq = ptr1->frequency;
                ptr1->frequency = ptr2->frequency;
                ptr2->frequency = tempFreq;

                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}
void find_word_frequency(char *str, struct hashElement **head)
{
    char word[100]; // Buffer for word
    int i = 0;
    int frequency = 0;
    // printf("%s", str);
    while (*str)
    {
        // Reset word buffer
        memset(word, 0, sizeof(word));
        i = 0;
        // Read word
        while (isalpha(*str) && i < sizeof(word) - 1)
        {
            word[i++] = tolower(*str);
            str++;
        }
        word[i] = '\0'; // Null-terminate the word
        frequency = 0;
        // Read frequency number
        while (isdigit(*(str)))
        {

            frequency = frequency * 10 + (*str - '0');
            str++;
        }

        insert_or_update(head, word, frequency, strlen(word));
    }
}

/*LLM generated -> There was a problem with detecting the last word from the text */
int find_last_word(char *buffer, int start, int limit_size)
{
    int pos = -1; // Position of the last word found
    int word_start = -1;
    int buffer_length = strlen(buffer);

    if (start >= buffer_length)
    {
        printf("Find last word at position ");
        return -1; // Out of bounds
    }

    // Ensure we don't exceed buffer length
    int end = (start + limit_size < buffer_length) ? (start + limit_size) : (buffer_length);

    for (int i = start; i < end; i++)
    {
        if (isalpha(buffer[i]))
        {
            if (word_start == -1)
                word_start = i; // Mark the start of a new word
        }
        else if (word_start != -1)
        {
            pos = word_start; // Store position of the last found word
            word_start = -1;  // Reset for next potential word
        }
    }

    /*End of read text*/
    if (end == buffer_length)
        return strlen(buffer);
    // If a word was being read at the end, update position
    if (word_start != -1)
        pos = word_start;

    return pos; // Return start position of last word, or -1 if none found
}

void divide_text(struct thread_args distributor_thread_args[], int argc, int *worker_port, char *text_data)
{
    int number_of_thread = argc - 2;
    size_t const chunk_size = msg_len - 4; // also count null terminator
    /*Distribute text equally, How many character should each worker get*/
    int start_index = 0;
    int end_index = 0;
    int i = 0; // Start indexing from 0
    for (int thread = 0; thread < number_of_thread; thread++)
    {
        distributor_thread_args[thread].number_of_chunks = 0;
        distributor_thread_args[thread].worker_port = worker_port[thread];
        distributor_thread_args[thread].buffer_chunk = malloc(sizeof(char *)); // Freed 426
    }
    int thread_number = 0;
    int last_word = -1;
    /*Finding a chunk and assign it to each worker. Each worker takes turn to receive chunk*/
    while (start_index < strlen(text_data))
    {
        thread_number = i % number_of_thread;
        last_word = find_last_word(text_data, start_index, chunk_size);
        if (last_word < 0)
        {
            /*Text is completely processed*/
            break;
        }
        else
        {
            end_index = last_word;
            // Reallocate memory for the new chunk
            int chunk_number = distributor_thread_args[thread_number].number_of_chunks;
            char **new_realloc = realloc(distributor_thread_args[thread_number].buffer_chunk, (sizeof(char *)) * (chunk_number + 1));

            if (new_realloc == NULL)
            {
                perror("Failed to reallocate memory for buffer_chunk");
                exit(EXIT_FAILURE);
            }

            distributor_thread_args[thread_number].buffer_chunk = new_realloc;

            if (distributor_thread_args[thread_number].buffer_chunk == NULL)
            {
                perror("Failed to allocate memory for buffer_chunk");
                exit(EXIT_FAILURE);
            }

            // Copy the text into chunk
            char temp[chunk_size];
            strncpy(temp, text_data + start_index, end_index - start_index);
            temp[end_index - start_index] = '\0';

            distributor_thread_args[thread_number].buffer_chunk[chunk_number] = strdup(temp);
            distributor_thread_args[thread_number].number_of_chunks++;

            if (distributor_thread_args[thread_number].buffer_chunk[chunk_number] == NULL)
            {
                perror("Failed to allocate memory for chunk");
                exit(EXIT_FAILURE);
            }
        }
        start_index = end_index;
        i++;
    }
    return;
}

void send_file_chunks(void *context, char **worker_ports, int num_workers, const char *text_data)
{

    struct thread_args distributor_thread_args[num_workers];
    void **sockets = malloc(num_workers * sizeof(void *));
    for (int i = 0; i < num_workers; i++)
    {
        sockets[i] = zmq_socket(context, ZMQ_REQ);
        char endpoint[50];
        snprintf(endpoint, sizeof(endpoint), "tcp://127.0.0.1:%s", worker_ports[i]);
        zmq_connect(sockets[i], endpoint);
    }
    /*Distribute text into chunk and assign to worker iterative manners*/
    divide_text(distributor_thread_args, num_workers + 2, worker_ports, text_data);
    /*Create connection to worker*/
    char buffer_request_msg[msg_len];
    int bytes_received = -1;
    char reply[msg_len];
    // Send a request to each worker
    for (int worker = 0; worker < num_workers; worker++)
    {

        for (int i = 0; i < distributor_thread_args[worker].number_of_chunks; i++)
        {
            /*Make sure to spare space for intstruction and null terminator*/
            int available_space = msg_len - 4; // "map" (3 chars) + null terminator (1 char)
            /*Recommended from LLM*/
            snprintf(buffer_request_msg, msg_len, "map%.*s", available_space, distributor_thread_args[worker].buffer_chunk[i]);
            zmq_send(sockets[worker], buffer_request_msg, strlen(buffer_request_msg) + 1, 0);
            bytes_received = zmq_recv(sockets[worker], reply, msg_len, 0);

            if (bytes_received == -1)
            {
                perror("zmq_recv failed");
                break;
            }
            reply[bytes_received] = '\0'; //  make sure Null-terminate
            /*It was a critical problem here, tried to reassign the string without properly reallocating the space again */
            distributor_thread_args[worker].buffer_chunk[i] = realloc(distributor_thread_args[worker].buffer_chunk[i], sizeof(reply));
            strcpy(distributor_thread_args[worker].buffer_chunk[i], reply);

            memset(reply, 0, sizeof(reply));
            memset(buffer_request_msg, 0, sizeof(buffer_request_msg));
        }
    }
    for (int worker = 0; worker < num_workers; worker++)
    {
        for (int i = 0; i < distributor_thread_args[worker].number_of_chunks; i++)
        {
            /*Make sure to spare space for intstruction and null terminator*/
            int available_space = msg_len - 4; // "red" (3 chars) + null terminator (1 char)
            /*Recommended from LLM*/
            snprintf(buffer_request_msg, msg_len, "red%.*s", available_space, distributor_thread_args[worker].buffer_chunk[i]);
            zmq_send(sockets[worker], buffer_request_msg, strlen(buffer_request_msg) + 1, 0);

            bytes_received = zmq_recv(sockets[worker], reply, msg_len, 0);

            if (bytes_received == -1)
            {
                perror("zmq_recv failed");
                break;
            }
            reply[bytes_received] = '\0'; // Null-terminate

            distributor_thread_args[worker].buffer_chunk[i] = realloc(distributor_thread_args[worker].buffer_chunk[i], sizeof(reply));
            strcpy(distributor_thread_args[worker].buffer_chunk[i], reply);

            memset(reply, 0, strlen(reply));
            memset(buffer_request_msg, 0, sizeof(buffer_request_msg));
        }
    }
    for (int worker = 0; worker < num_workers; worker++)
    {

        int total_size = 1; // Start with 1 for the null terminator
        for (int i = 0; i < distributor_thread_args[worker].number_of_chunks; i++)
        {
            total_size += strlen(distributor_thread_args[worker].buffer_chunk[i]);
        }
        // Allocate and initialize the result buffer
        distributor_thread_args[worker].result = malloc(total_size); // Freed in 441
        /*Make sure it is empty*/
        memset(distributor_thread_args[worker].result, 0, total_size);
        // Sending rip with nullterminator
        bytes_received = zmq_send(sockets[worker], "rip\0", 4, 0);
        if (bytes_received > 0)
        {
            reply[bytes_received] = '\0'; // Null-terminate
        }

        zmq_recv(sockets[worker], reply, msg_len, 0);

        for (int i = 0; i < distributor_thread_args[worker].number_of_chunks; i++)
        {
            strcat(distributor_thread_args[worker].result, distributor_thread_args[worker].buffer_chunk[i]);
            free(distributor_thread_args[worker].buffer_chunk[i]);
        }

        free(distributor_thread_args[worker].buffer_chunk);
    }
    /*Finding length of result of each worker*/
    int total_message = 1; // Start with 1 for the null terminator
    for (int i = 0; i < num_workers; i++)
    {
        total_message += (strlen(distributor_thread_args[i].result) + 1);
    }
    char *result = malloc(total_message);     // Freed 454
    char *msg_buffer = malloc(total_message); // Freed 455
    memset(result, 0, total_message);
    result[0] = '\0'; /*Empty String*/
    for (int i = 0; i < num_workers; i++)
    {
        strcat(result, distributor_thread_args[i].result);
        free(distributor_thread_args[i].result);
        zmq_close(sockets[i]);
    }
    strcat(result, "\0");
    struct hashElement *head = NULL;
    find_word_frequency(result, &head);
    sortList(&head);

    printf("word,frequency\n");
    /*Printing the word,frequency*/
    printList_frequency(head, msg_buffer, total_message);
    /*Clean up the linked List and buffer*/
    freeList(&head);
    free(msg_buffer);
    free(result);
    free(sockets);
    return;
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <file.txt> <worker port 1> <worker port 2> ... <worker port n>\n", argv[0]);
        return 1;
    }

    void *context = zmq_ctx_new();

    FILE *file = fopen(argv[1], "r"); // Open file in read mode
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    // Seek to the end to get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file); // Get file size
    rewind(file);                 // Reset file pointer to beginning
    // Allocate buffer dynamically based on file size
    char *buffer = malloc(file_size + 1); // Freed 616
    if (buffer == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }
    // Read file content into buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    send_file_chunks(context, &argv[2], argc - 2, buffer);
    zmq_ctx_destroy(context);
    free(buffer); // Free allocated memory
    fclose(file); // Close file
    return 0;
}
