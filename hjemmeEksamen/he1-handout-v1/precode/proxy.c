/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

/* This struct should contain the information that you want
 * keep for one connected client.
 * Clients are used as a linked list/stack.
 */
struct Client {
    struct Client *next;
    int sock;
};

typedef struct Client Client;

void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <port>\n"
                     "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                     "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                     "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
 struct Client* handleNewClient( int server_sock, struct Client* start ) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    //Accept connection
    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock == -1) {
        perror("Error accepting client connection. Function: handleNewClient");
        return start;
    } 
    
    //Create new client
    Client *client = malloc(sizeof(Client));
    if (!client) {
        perror("Could not allocate memory for new client.");
        return start;
    }
    client->next = start;
    client->sock = client_sock;

    //Log info
    printf("New client connected. Socket no. : %d\n", client->sock);
    
    //returns new client* to be used as start
    return client;
    
}


/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 * If start is removed, new start is returned. Else start will be returned.
 */
struct Client*  removeClient( Client* client, Client* start ) {
    if (!client || !start) {
        perror("Either you have no clients or the client to be removed doesent exist");
        return client ? client : start;

    } 
    struct Client* curr;
    if (start->sock == client->sock) {
        curr = client->next;
        close(client->sock);
        free(client);
        return curr;
    }
    curr = start; 
    while (curr) {
        if (curr->next || curr->next->sock == client->sock) {
            curr->next = curr->next->next;
            close(curr->next->sock);
            free(curr->next);
            return start;
        } else {
            curr = curr->next;
        }
    }
    return start;
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forwardMessage( Record* msg ) {

}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handleClient( Client* client ) {
}

int main( int argc, char* argv[] ) {
    int port;
    int server_sock;

    if( argc != 2 )
    {
        usage( argv[0] );
    }

    port = atoi( argv[1] );

    server_sock = tcp_create_and_listen( port );
    if( server_sock < 0 ) exit( -1 );

    /* add your initialization code */
    
    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    do
    {
        /* fill in your code */
    }
    while( 1 /* fill in your termination condition */ );

    /* add your cleanup code */

    tcp_close( server_sock );

    return 0;
}

