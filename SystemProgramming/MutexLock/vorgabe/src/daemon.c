#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "../include/daemon.h"
#include "../include/ringbuf.h"

#define WAIT_TIME 1000 // usec

/* IN THE FOLLOWING IS THE CODE PROVIDED FOR YOU
 * changing the code will result in points deduction */

/********************************************************************
 * NETWORK TRAFFIC SIMULATION:
 * This section simulates incoming messages from various ports using
 * files. Think of these input files as data sent by clients over the
 * network to our computer. The data isn't transmitted in a single
 * large file but arrives in multiple small packets. This concept
 * is discussed in more detail in the advanced module:
 * Rechnernetze und Verteilte Systeme
 *
 * To simulate this parallel packet-based data transmission, we use multiple
 * threads. Each thread reads small segments of the files and writes these
 * smaller packets into the ring buffer. Between each packet, the
 * thread sleeps for a random time between 1 and 100 us. This sleep
 * simulates that data packets take varying amounts of time to arrive.
 *********************************************************************/

typedef struct
{
    rbctx_t *ctx;
    connection_t *connection;
} w_thread_args_t;

typedef struct
{
    rbctx_t *ctx;
} r_thread_args_t;

// declare mutex and cond for each PORT
pthread_mutex_t mutex[MAXIMUM_PORT];
pthread_cond_t sig[MAXIMUM_PORT];

size_t latest_packets[MAXIMUM_PORT];

void *write_packets(void *arg)
{
    /* extract arguments */
    rbctx_t *ctx = ((w_thread_args_t *)arg)->ctx;
    size_t from = (size_t)((w_thread_args_t *)arg)->connection->from;
    size_t to = (size_t)((w_thread_args_t *)arg)->connection->to;
    char *filename = ((w_thread_args_t *)arg)->connection->filename;

    /* open file */
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open file with name %s\n", filename);
        exit(1);
    }

    /* read file in chunks and write to ringbuffer with random delay */
    unsigned char buf[MESSAGE_SIZE];
    size_t packet_id = 0;
    size_t read = 1;
    while (read > 0)
    {
        size_t msg_size = MESSAGE_SIZE - 3 * sizeof(size_t);
        read = fread(buf + 3 * sizeof(size_t), 1, msg_size, fp);
        if (read > 0)
        {
            memcpy(buf, &from, sizeof(size_t));
            memcpy(buf + sizeof(size_t), &to, sizeof(size_t));
            memcpy(buf + 2 * sizeof(size_t), &packet_id, sizeof(size_t));
            while (ringbuffer_write(ctx, buf, read + 3 * sizeof(size_t)) != SUCCESS)
            {
                usleep(((rand() % 50) + 25)); // sleep for a random time between 25 and 75 us
            }
        }
        packet_id++;
        usleep(((rand() % (100 - 1)) + 1)); // sleep for a random time between 1 and 100 us
    }
    fclose(fp);
    return NULL;
}

/* END OF PROVIDED CODE */

/********************************************************************/

/* YOUR CODE STARTS HERE */
void *read_packets(void *arg)
{
    /* extract arguments */
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    rbctx_t *context = ((r_thread_args_t *)arg)->ctx;
    assert(context != NULL);
    unsigned char buf[MESSAGE_SIZE];
    size_t read = MESSAGE_SIZE;

    while (1)
    {
        int retval = ringbuffer_read(context, buf, &read);
        // try to read until it succeeds
        while (retval == RINGBUFFER_EMPTY || retval == OUTPUT_BUFFER_TOO_SMALL)
        {
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            usleep(WAIT_TIME);
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            read = MESSAGE_SIZE;
            int retval = ringbuffer_read(context, buf, &read);
        }

        if (retval == SUCCESS)
        {
            size_t from_port;
            size_t to_port;
            size_t packet_id;
            size_t message_length = read - 3 * sizeof(size_t); // -from_port - to_port - packet_id

            // extrach Information from the Message
            char message[message_length + 1];
            memcpy(&from_port, buf, sizeof(size_t));
            memcpy(&to_port, buf + sizeof(size_t), sizeof(size_t));
            memcpy(&packet_id, buf + 2 * sizeof(size_t), sizeof(size_t));
            memcpy(&message, buf + 3 * sizeof(size_t), message_length);
            message[message_length] = '\0';

            pthread_mutex_lock(&mutex[to_port]);

            // check malicious
            char *malicious = "malicious";
            char *message_tmp = message;
            while (*message_tmp != '\0')
            {
                if (*message_tmp == *malicious)
                {
                    malicious++;
                }
                message_tmp++;
            }

            int condition_1 = (from_port == to_port) ? 1 : 0;
            int condition_2 = (from_port == 42 || to_port == 42) ? 1 : 0;
            int condition_3 = (from_port + to_port == 42) ? 1 : 0;
            int condition_4 = (*malicious == '\0') ? 1 : 0;

            // printf("from Port: %zu\n", from_port);
            // printf("To Port: %zu\n", to_port);
            // printf("Packet ID: %zu\n", packet_id);
            // printf("latest packet: %zu\n", latest_packets[to_port]);
            // printf("This packet: %zu\n", packet_id);

            // Assuring the right PACKET ID order of each packet
            while (packet_id != latest_packets[to_port] + 1 && packet_id != 0)
            // while (packet_id != latest_packets[to_port])

            {
                printf("Waiting for correct sequence...\n");
                pthread_cond_wait(&sig[to_port], &mutex[to_port]);
            }

            if (condition_1 || condition_2 || condition_3 || condition_4)
            {
                latest_packets[to_port] = packet_id; // skipped packet
                // latest_packets[to_port]++;

                pthread_cond_broadcast(&sig[to_port]);
                pthread_mutex_unlock(&mutex[to_port]);

                continue;
            }

            latest_packets[to_port] = packet_id;
            // latest_packets[to_port]++;

            char destination_path[10];
            snprintf(destination_path, sizeof(destination_path), "%zu.txt", to_port);
            FILE *fp_dst = fopen(destination_path, "a");
            if (fp_dst == NULL)
            {
                pthread_cond_broadcast(&sig[to_port]);
                pthread_mutex_unlock(&mutex[to_port]);

                exit(1);
            }
            // write in the destination file
            fwrite(message, sizeof(*message), message_length, fp_dst);
            fclose(fp_dst);
            pthread_cond_broadcast(&sig[to_port]);
            pthread_mutex_unlock(&mutex[to_port]);
        }
    }
    return NULL;
}
int simpledaemon(connection_t *connections, int nr_of_connections)
{
    /* initialize ringbuffer */
    rbctx_t rb_ctx;
    size_t rbuf_size = 1024;
    void *rbuf = malloc(rbuf_size);
    if (rbuf == NULL)
    {
        fprintf(stderr, "Error allocation ringbuffer\n");
    }

    ringbuffer_init(&rb_ctx, rbuf, rbuf_size);
    /****************************************************************
     * WRITER THREADS
     * ***************************************************************/
    /* prepare writer thread arguments */
    w_thread_args_t w_thread_args[nr_of_connections];
    for (int i = 0; i < nr_of_connections; i++)
    {
        w_thread_args[i].ctx = &rb_ctx;
        w_thread_args[i].connection = &connections[i];
        /* guarantee that port numbers range from MINIMUM_PORT (0) - MAXIMUMPORT */
        if (connections[i].from > MAXIMUM_PORT || connections[i].to > MAXIMUM_PORT ||
            connections[i].from < MINIMUM_PORT || connections[i].to < MINIMUM_PORT)
        {
            fprintf(stderr, "Port numbers %d and/or %d are too large\n", connections[i].from, connections[i].to);
            exit(1);
        }
    }

    /* start writer threads */
    pthread_t w_threads[nr_of_connections];
    for (int i = 0; i < nr_of_connections; i++)
    {
        pthread_create(&w_threads[i], NULL, write_packets, &w_thread_args[i]);
    }

    /* END OF PROVIDED CODE */
    /********************************************************************/
    /****************************************************************
     * READER THREADS
     * ***************************************************************/
    r_thread_args_t r_thread_args[NUMBER_OF_PROCESSING_THREADS];
    // Declare pthread_Mutex udn Cond  for every possible Port

    /* YOUR CODE STARTS HERE */

    // 1. think about what arguments you need to pass to the processing threads
    for (int i = 0; i < MAXIMUM_PORT; i++)
    {
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&sig[i], NULL);
    }
    for (int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++)
    {
        r_thread_args[i].ctx = &rb_ctx;
    }

    pthread_t r_threads[NUMBER_OF_PROCESSING_THREADS];

    // 2. start the processing threads
    for (int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++)
    {
        pthread_create(&r_threads[i], NULL, read_packets, &r_thread_args[i]);
        printf("Reader Thread created and started\n");
    }

    /* YOUR CODE ENDS HERE */

    /********************************************************************/

    /* IN THE FOLLOWING IS THE CODE PROVIDED FOR YOU
     * changing the code will result in points deduction */

    /****************************************************************
     * CLEANUP
     * ***************************************************************/

    /* after 5 seconds JOIN all threads (we should definitely have received all messages by then) */
    printf("daemon: waiting for 5 seconds before canceling reading threads\n");
    sleep(5);
    for (int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++)
    {
        pthread_cancel(r_threads[i]);
    }

    /* wait for all threads to finish */
    for (int i = 0; i < nr_of_connections; i++)
    {
        pthread_join(w_threads[i], NULL);
    }

    /* join all threads */
    for (int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++)
    {
        pthread_join(r_threads[i], NULL);
    }

    /* END OF PROVIDED CODE */

    /********************************************************************/

    /* YOUR CODE STARTS HERE */

    // use this section to free any memory, destory mutexe etc.
    for (int i = 0; i < MAXIMUM_PORT; i++)
    {
        pthread_mutex_destroy(&mutex[i]);
        pthread_cond_destroy(&sig[i]);
    }
    /* YOUR CODE ENDS HERE */

    /********************************************************************/

    /* IN THE FOLLOWING IS THE CODE PROVIDED FOR YOU
     * changing the code will result in points deduction */

    free(rbuf);
    ringbuffer_destroy(&rb_ctx);

    return 0;

    /* END OF PROVIDED CODE */
}