#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <pthread.h>

#define msg_len 1500

struct thread_args
{
    int worker_port;
    void *context;
};
struct hashElement
{
    char *key;
    int frequency;
    struct hashElement *next;
};

void toLowerCase(char *str)
{
    while (*str != '\0')
    {
        *str = tolower(*str);
        str++;
    }
}

// Function to free the linked list
void freeList(struct hashElement **head_ref)
{
    struct hashElement *current = *head_ref;
    struct hashElement *next;

    while (current != NULL)
    {
        next = current->next;
        free(current->key); // Free the key of current node
        free(current);      // Free the current node
        current = next;
    }

    *head_ref = NULL; // Set head to NULL after freeing
}

void worker_thread_join(pthread_t *worker_thread, int argc)
{
    for (int i = 0; i < argc - 1; i++)
    {
        if (pthread_join(worker_thread[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }
    return;
}

/*LLM helped with structure*/
void extractWords(const char *str, struct hashElement **head)
{
    char word[100]; // Buffer for storing words
    int i = 0;

    while (*str)
    {
        if (isalpha(*str))
        { // If it's a letter, add to word buffer
            word[i++] = tolower(*str);
        }
        else if (i > 0)
        {
            // If a non-alphabetic char is found, terminate the word
            word[i] = '\0'; // Null-terminate the word
            insert_or_update(head, word, 1);
            i = 0; // Reset for the next word
        }
        str++; // Move to next character
    }
    if (i > 0)
    {
        word[i] = '\0';
        insert_or_update(head, word, 1);
    }
}

void find_word_1_frequency(const char *str, struct hashElement **head)
{
    char word[100];
    int frequency = 0;
    int i = 0;
    word[0] = '\0';
    while (*str)
    {
        if (isalpha(*str))
        { // If it's a letter, add to word buffer
            word[i++] = tolower(*str);
            str++;
        }
        else
        {
            if (i > 0) // We found a non-alphabetic character and completed a word
            {
                word[i] = '\0'; // Null-terminate the word
                while (*str == '1')
                {
                    str++;
                    frequency++;
                }
                insert_or_update(head, word, frequency);
                i = 0; // Reset for the next word
                frequency = 0;
            }
        }
    }

    // Check if there's a word left at the end of the string likely not happening
    if (i > 0)
    {
        word[i] = '\0'; // Null-terminate the last word
        while (*str == '1')
        {
            str++;
            frequency++;
        }
        insert_or_update(head, word, frequency);
    }
}

void *worker_thread_function(void *arg)
{
    struct thread_args *worker_arg = (struct thread_args *)arg;
    void *responder = zmq_socket(worker_arg->context, ZMQ_REP); // freed
    int linger = 0;
    if (zmq_setsockopt(responder, ZMQ_LINGER, &linger, sizeof(linger)) != 0)
    {
        printf("Error setting LINGER option: %s\n", zmq_strerror(zmq_errno()));
        return;
    }
    char msg_buffer[msg_len];
    char buffer[msg_len];
    char response[256];
    int bytes_received = -1;
    snprintf(response, sizeof(response), "tcp://*:%d", worker_arg->worker_port);

    int rc = zmq_bind(responder, response);
    if (rc == -1)
    {
        // If zmq_bind fails, print the error message
        printf("zmq_bind failed: %s\n", zmq_strerror(zmq_errno()));
        zmq_close(responder);
        return NULL;
    }

    struct hashElement *head = NULL;
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = zmq_recv(responder, buffer, msg_len, 0);
        if (bytes_received == -1)
        {
            perror("zmq_recv failed");
            break;
        }
        buffer[bytes_received] = '\0'; /*Terminate recv, incase it is not null terminated*/
        if (strncmp(buffer, "map", 3) == 0)
        {

            memset(msg_buffer, 0, sizeof(msg_buffer));
            extractWords(buffer + 3, &head);
            printList_1(head, msg_buffer);

            zmq_send(responder, msg_buffer, strlen(msg_buffer) + 1, 0);
            freeList(&head);
        }
        else if (strncmp(buffer, "red", 3) == 0)
        {

            memset(msg_buffer, 0, sizeof(msg_buffer));
            find_word_1_frequency(buffer + 3, &head);
            printList_frequency(head, msg_buffer);
            zmq_send(responder, msg_buffer, strlen(msg_buffer) + 1, 0);
            freeList(&head);
        }
        else if (strncmp(buffer, "rip", 3) == 0)
        {
            // printf("It is rip request\n");
            zmq_send(responder, "rip\0", 4, 0);
            break;
        }
    }

    zmq_close(responder);
    free(arg);

    return NULL;
}

void worker_thread_setup(pthread_t *worker_thread, struct thread_args *worker_thread_args, int argc, int *worker_port, void *context)
{
    int number_of_threads = argc - 1;

    // Ensure we're only working within the valid range of arguments
    for (int i = 0; i < number_of_threads; i++)
    {
        worker_thread_args[i].worker_port = worker_port[i + 1]; // Assign worker port from argv
        worker_thread_args[i].context = context;                // Assign worker port from argv
    }

    // Create threads for each worker
    for (int i = 0; i < number_of_threads; i++)
    {
        // Dynamically allocate memory for each thread's arguments
        struct thread_args *thread_args_copy = malloc(sizeof(struct thread_args));
        if (thread_args_copy == NULL)
        {
            perror("Failed to allocate memory for thread args");
            return;
        }

        // Copy the current thread argument
        *thread_args_copy = worker_thread_args[i];

        // Create thread, passing the pointer to the argument structure
        if (pthread_create(&worker_thread[i], NULL, worker_thread_function, (void *)thread_args_copy) != 0)
        {
            perror("Failed to create thread");
            free(thread_args_copy); // Free memory if thread creation fails
            return;
        }
    }
    return;
}
void printList_1(struct hashElement *head, char *msg)
{
    struct hashElement *temp = head;
    while (temp != NULL)
    {
        // printf("(%s,%d), ", temp->key, temp->frequency);
        strcat(msg, temp->key);
        for (int i = 0; i < temp->frequency; i++)
        {
            strcat(msg, "1");
        }
        temp = temp->next;
    }
    /*Always terminate String*/
    strcat(msg, "\0");
}
void printList_frequency(struct hashElement *head, char *msg)
{
    struct hashElement *temp = head;
    char msg_temp[msg_len];
    while (temp != NULL)
    {
        snprintf(msg_temp, sizeof(msg_temp), "%s%d", temp->key, temp->frequency);
        strcat(msg, msg_temp);
        temp = temp->next;
    }
    /*Always terminate String*/

    strcat(msg, "\0");
}
struct hashElement *createNode(char *key, int frequency)
{
    struct hashElement *newNode = (struct hashElement *)malloc(sizeof(struct hashElement));
    // newNode->key = (struct hashElement *)malloc(sizeof(char) * (key_len + 1));
    newNode->key = strdup(key);

    // strcpy(newNode->key, key);
    newNode->frequency = frequency;
    newNode->next = NULL;
    return newNode;
}
/*generated from LLM*/
void insert_or_update(struct hashElement **head, char *new_key, int frequency)
{
    // Case 1: If the list is empty, insert the first node
    if (*head == NULL)
    {
        struct hashElement *newNode = createNode(new_key, frequency); // Creating the first node
        *head = newNode;                                              // Set the head to point to the new node
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
    struct hashElement *newNode = createNode(new_key, frequency); // Create a new node with frequency 1
    temp = *head;                                                 // Start from the head again
    while (temp->next != NULL)
    {
        temp = temp->next; // Traverse until the end
    }
    temp->next = newNode; // Insert the new node at the end
    return;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1; /*Arguments Failure*/
    }

    int port_worker[argc];

    for (int i = 1; i < argc; i++)
    {
        // printf(" %s ", argv[i]);
        port_worker[i] = atoi(argv[i]); /*String to Int*/
    }

    pthread_t worker_thread[argc - 1];
    struct thread_args *worker_thread_args = (struct thread_args *)malloc(sizeof(struct thread_args) * (argc - 1)); // Freed
    void *context = zmq_ctx_new();                                                                                  // freed

    worker_thread_setup(worker_thread, worker_thread_args, argc, port_worker, context);
    if (worker_thread_args == NULL)
    {
        perror("Memory allocation failed for worker_thread_args");
        exit(1); // Terminate the program or handle the error gracefully
    }
    worker_thread_join(worker_thread, argc);
    zmq_ctx_destroy(context);
    free(worker_thread_args);
    return 0;
}
