/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */

#ifndef TWITTERTRENDSERVER_H
#define TWITTERTRENDSERVER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "makeargv.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


/* max number of clients */
#define MAX_CLIENT 100
#define HANDSHAKE  100
#define HANDSHAKERESPONSE 101
#define CLIENTREQUEST 102
#define TRENDREPLY    103
#define ENDOFREQUEST  104
#define ENDOFRESPONSE 105
#define ERROR         106
/* usefull into for printing */
#define APPENDSTR ".result"
#define COMMA     ","
#define NEWLINE   "\n"
#define NULLEND   0

/*define error checking function*/
#define COND_CHECK(func, cond, retv, errv)                              \
  if ( (cond) )                                                         \
    {                                                                   \
      fprintf(stderr, "\n[CHECK FAILED at %s:%d]\n| %s(...) = %d (%s)\n\n", \
              __FILE__,__LINE__,func,retv,strerror(errv));              \
      exit(EXIT_FAILURE);                                               \
    }

#define ErrnoCheck(func,cond,retv)  COND_CHECK(func, cond, retv, errno)
#define PthreadCheck(func,rc) COND_CHECK(func,(rc!=0), rc, rc)
#define MAX_SIZE 5
#define DATABASE "TwitterDB.txt"
#define DEFAULTTHREADS 5
#define MAX_LENGTH 256
#define MAX_BUF 200 // Every line in TwitterDB.txt is less than 100 characters.
#define MAX_CITY 15 // cityName are less than 15 characters
#define MAPSIZE 100 // max clients number
#define QUOTE "\""
#define NOTFOUND  "NA"
/*define new data type*/
typedef char* string;
typedef int   bool;
/* define a node structure */
struct twitterDBEntry {
  char cityName[MAX_CITY];
  char keywords[MAX_BUF];
};

/* for thread ID */
int portNum, serverfd, rc, clientPort[MAX_SIZE];
struct sockaddr_in server, client;
/*define global variables for threads*/
int numThread; // bounded buffer size
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
int taskQueue[MAX_CLIENT];
int activeTaskCounter = 0;
int currentTaskCounter = 0;
int payload = 0;
struct twitterDBEntry twitterDB[MAPSIZE];
char clientAddr[MAX_CLIENT][MAX_CITY];
int twitterDBCounter = 0;
char message[MAX_LENGTH], trends[MAX_BUF], cityName[MAX_CITY];


/* prototype goes here */
int twitterTrendServer( int numThread );
void createThread( int numThread );
void* connectionHandler(void* threadId);
void populateTaskQueue();
int extractThreadId(pthread_t ids);
void messageHandler(int clientfd);
int malformedMsgChecker();
int handshakeResponse(int clientfd);
void replyClient(int clientfd);
int endCheck(int msgId, int clientfd);
void errorSet(int clientfd);
FILE* openFile( string fileName );
void makeMap( FILE* fp );
void printTwitterDB();
void addToTwitterDB();
int lookupTwitterDB(char cityName[], char keywords[]);

#endif // TWITTERTRENDSERVER_H
