/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */



#include "../include/twitterTrendServer.h"

/* main function in twitterTrend file */
int twitterTrendServer( int numThread ) {
  /* create socket */
  int true = 1;
  serverfd = socket(PF_INET, SOCK_STREAM, 0);
  ErrnoCheck("Failed to create socket", (rc < 0), rc);
  setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (char*) & true,  sizeof(true));
  ErrnoCheck("Fail to set reuse mode", (rc < 0), rc);
  /* Prepare the sockaddr_in */
  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(portNum);

  /* Bind */
  rc = bind(serverfd, (struct sockaddr *)& server, sizeof(server));
  ErrnoCheck("Failed to bind", (rc < 0), rc);

  /* Listen */
  rc = listen(serverfd, numThread);
  ErrnoCheck("Failed to listen", (rc < 0), rc);

  createThread( numThread );
  return 0;
}

/*
 * main - main thread and will create as many threads as specified from terminal
 */
void createThread( int numThread ) {
  int i;
  int tids[numThread];
  pthread_t threads[numThread];
  /*
   * create threads for clients as numThread said
   */
  for ( i = 0; i < numThread; i++ ) {
    /* run handleClient concurrently */
    tids[i] = i + 1;
    rc = pthread_create(&threads[i],
                        NULL,
                        connectionHandler,
                        (void*)& tids[i]);
    PthreadCheck("pthread_create", rc);
  }

  /* run producer in main thread */

  populateTaskQueue();

  /* wait thread till it finish */
  for ( i = 0; i < numThread; ++i ) {
    rc = pthread_join( threads [i], NULL);
    PthreadCheck("pthread_join", rc);
  }

}

/* This is comsumer function */
void* connectionHandler( void* arg ) {
  int counter = 0, clientfd, port;
  int tid = *(int *)arg;
  char clientaddr[MAX_CITY];
  while(1) {
    pthread_mutex_lock(&queue_mutex);
    if (activeTaskCounter > 0) {
      activeTaskCounter -= 1;
      counter = activeTaskCounter;
      clientfd = taskQueue[counter];
      port = clientPort[counter];
      strcpy(clientaddr, clientAddr[counter]);
      pthread_mutex_unlock(&queue_mutex);

      printf("Thread %d is handling client %s %d\n", tid, clientaddr, port);
      messageHandler(clientfd);
      usleep(10000);
      printf("Thread %d finished handling client %s %d\n", tid, clientaddr, port);
    }
    pthread_mutex_unlock(&queue_mutex);
    pthread_mutex_lock(&client_mutex);
  }
  return 0;
}

void populateTaskQueue() {
  int clientfd;
  int c = sizeof(struct sockaddr_in);
  struct sockaddr_in client;

  while ((clientfd = accept(serverfd, (struct sockaddr *)&client, (socklen_t*)&c ))) {
    pthread_mutex_lock(&queue_mutex);
    ErrnoCheck("Failed to accept",  (clientfd < 0), clientfd );
    taskQueue[activeTaskCounter] = clientfd;
    clientPort[activeTaskCounter] = client.sin_port;
    strcpy (clientAddr[activeTaskCounter], inet_ntoa(client.sin_addr));
    activeTaskCounter += 1;
    pthread_mutex_unlock(&queue_mutex);
    pthread_mutex_unlock(&client_mutex);
  }
}

void messageHandler(int clientfd) {
  int msgId;
  while (handshakeResponse(clientfd));

  while (1) {
    rc = read(clientfd, message, MAX_LENGTH);
    ErrnoCheck("Failed to read to socket", (rc < 0), rc);

    if (malformedMsgChecker()) {
      errorSet(clientfd);
    }
    msgId = atoi(message + 1);
    if (msgId == CLIENTREQUEST){
      replyClient(clientfd);
    }

    if (msgId == ERROR) {
      fprintf(stderr, "Error happened on Client side");
      bzero(message, MAX_LENGTH);
      return;
    }

    if (endCheck(msgId, clientfd))
      return;
    bzero(message, MAX_LENGTH);
 }
}

int malformedMsgChecker() {
  string* msgvector;
  string* cityname;
  if (message[0] != '(') {
    return 1;
  }
  else if (message[strlen(message) - 1] != ')') {
    return 1;
  }
  else {
    if ((payload = atoi(message + 5))) {
      // test comma, shall be larger than 2
      if(makeargv(message, COMMA, &msgvector) < 1) {
        return 1;
      }
      else {
        makeargv(message, QUOTE, &cityname);
        if (payload != (int)strlen(cityname[1])) {
          return 1;
        }
        else {
          strncpy(cityName, cityname[1], payload);
        }
        freemakeargv(msgvector);
        freemakeargv(cityname);
      }
    }

  }
  return 0;
}

void errorSet(int clientfd){
  bzero(message, MAX_LENGTH);
  sprintf(message, "%s%d%s%d%s%s","(", ERROR, COMMA, 0, COMMA, ")\0");
  rc = write(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to write to socket", (rc < 0), rc);
  close(clientfd);
  exit(1);
}

int endCheck(int msgId, int clientfd) {
  if (msgId == ENDOFREQUEST) {
    close(clientfd);
    return 1;
  }
  return 0;
}
void replyClient(int clientfd) {
    /* retrive trends info associated with cith name */
  lookupTwitterDB(cityName, trends);
  bzero(cityName, strlen(cityName));
  /* make message info */
  sprintf(message,
          "%s%d%s%ld%s%s%s%s%s","(", TRENDREPLY, COMMA, strlen(trends),
          COMMA, QUOTE, trends, QUOTE,")\0");

  rc = write(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to write to socket", (rc < 0), rc);
  bzero(message, MAX_LENGTH);
  sprintf(message, "%s%d%s%d%s%s","(", ENDOFRESPONSE, COMMA, payload, COMMA, ")\0");
  rc = write(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to write to socket", (rc < 0), rc);
  bzero(message, MAX_LENGTH);
  bzero(trends, MAX_LENGTH);
}

int handshakeResponse(int clientfd) {
  bzero(message, MAX_LENGTH);
  sprintf(message, "%s%d%s%d%s%s",
          "(", HANDSHAKE, COMMA, 0, COMMA, ")\0");
  rc = write(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to write to socket", (rc < 0), rc);
  bzero(message, MAX_LENGTH);
  rc = read(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to read to socket", (rc < 0), rc);
  if (atoi(message + 1) == HANDSHAKERESPONSE)
    return 0;
  return 1;
}

int main(int argc, char *argv[]) {
  int numThread;
  if (argc < 2) {
    perror("Only accpet at least two arguments");
  }
  /* extract useful information from command line */
  portNum = atoi (argv[1]);
  numThread = DEFAULTTHREADS;

  if (argc == 3)
    numThread = atoi(argv[2]);
  /* open data base file */
  FILE* fp = openFile( DATABASE );

  /* for imporving the efficiency, stotre data in a linked list (map) */
  makeMap( fp );
  /* start to handle clients */
  twitterTrendServer( numThread );

  return 0;

}



void addToTwitterDB(struct twitterDBEntry entry) {

  strncpy(twitterDB[twitterDBCounter].cityName, entry.cityName, strlen(entry.cityName));
  strncpy(twitterDB[twitterDBCounter++].keywords, entry.keywords, strlen(entry.keywords));
}

int lookupTwitterDB(char cityName[], char keywords[]) {
  int i = 0, found = 0;
  for(i = 0; i < twitterDBCounter; i++) {
    if(!strncmp(twitterDB[i].cityName, cityName, strlen(cityName))) {
      strncpy(keywords, twitterDB[i].keywords,strlen(twitterDB[i].keywords));
      found = 1;
      break;
    }
  }
  if (!found) {
    bzero(keywords, strlen(keywords));
    strcpy(keywords, NOTFOUND);
    return 0;
  }
  return 1;
}

void printTwitterDB()
{
  int i = 0;
  for(i = 0; i < twitterDBCounter; i++) {
    printf("CityName: %s, TrendingKeyWords: %s\n",twitterDB[i].cityName, twitterDB[i].keywords);
  }
}
/*open file by filename*/
FILE* openFile( string fileName ) {
  FILE* fp = fopen(fileName, "r");
  if ( fp == NULL ) {
    perror("Failed to open file");
  }

  return fp;
}

/*read contents from database file
  each line start with city name and trends
  hash city name and store trend to bucket
*/
void makeMap( FILE* fp ) {
  char buffer[MAX_BUF];
  string token;
  struct twitterDBEntry entry;
  while ( fgets(buffer, MAX_BUF, fp) ) {
    token = strtok(buffer, ",");
    strncpy(entry.cityName, token, strlen(token));
    entry.cityName[strlen(token)]='\0';
    token = strtok(NULL, " ");
    strncpy(entry.keywords, token, strlen(token));
    entry.keywords[strlen(token)-1]='\0';
    addToTwitterDB(entry);
}
}
