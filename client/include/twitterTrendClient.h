/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */

#ifndef TWITTERTRENDCLIENT_H
#define TWITTERTRENDCLIENT_H


/* max number of clients */
#define MAX_CLIENT 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "alloc.h"
#include "makeargv.h"
#include <arpa/inet.h>

typedef char* string;

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
#define SocketCheck(func,rc) COND_CHECK(func,(rc < 0), rc, rc)
#define MAX_BUF 100
#define MAX_LENGTH 256
#define MAX_CITY 15
#define APPENDSTR ".result"
#define COLON     " : "
#define NEWLINE   "\n"
#define DELIMETER "\"" //for distinguish city and trends
#define NULLEND   0
#define HANDSHAKE  100
#define HANDSHAKERESPONSE 101
#define CLIENTREQUEST 102
#define TRENDREPLY    103
#define ENDOFREQUEST  104
#define ENDOFRESPONSE 105
#define ERROR         106
#define COMMA       ","
#define QUOTE       "\""
string clientPath[MAX_CLIENT];
string citys[MAX_CLIENT];
string hostname;
int portNum;
int clients, payload, rc;
struct sockaddr_in serv_addr;
char message[MAX_LENGTH], *trends;
string* keywords;
/* prototype goes here */
void twitterTrendClient( int numClient );
void praseClient( string path );
void connectionHandler(int d, int clientfd);
void write2File(string path, string cityName, string trends);
void messageHandler(int id, int clientfd);
int malformedMsgChecker();
void errorSet(int clientfd);
#endif // TWITTERTRENDCLIENT_H
