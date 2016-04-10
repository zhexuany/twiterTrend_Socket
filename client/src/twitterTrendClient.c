/* CSci4061 F2015 Assignment 4
 * Student: Zhexuan Zachary Yang &  Yuanting Shao
 * X500:     yang4442      &  shaox119
 * we did extra credits
 */



#include "../include/twitterTrendClient.h"

/* main function in citysTrend file */
void twitterTrendClient(int numClient) {
  int clientfd, rc, i, msgId;
  struct hostent *server;
  string message = MALLOC(MAX_LENGTH, char);
  clients = numClient;
  clientfd = socket(PF_INET, SOCK_STREAM, 0);

  server = gethostbyname(hostname);

  if (server == NULL) {
    fprintf(stderr,"No such host\n");
    exit(0);
  }

  bzero((char *) &serv_addr, sizeof (serv_addr));
  serv_addr.sin_family = PF_INET;
  bcopy((string)server -> h_addr, (string)&serv_addr.sin_addr.s_addr,
        server -> h_length);

  serv_addr.sin_port = htons(portNum);

  rc = connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  ErrnoCheck("Failed to connect", (rc < 0), rc);

  while (1) {
    bzero(message, MAX_LENGTH);
    rc = read(clientfd, message, MAX_LENGTH);
    ErrnoCheck("Failed to read to socket", (rc < 0), rc);

    msgId = atoi(message + 1);
    bzero(message, MAX_LENGTH);
    if (msgId == HANDSHAKE) {
      sprintf(message, "%s%d%s%d%s%s",
              "(", HANDSHAKERESPONSE, COMMA, 0, COMMA, ")\0");
      rc = write(clientfd, message, MAX_LENGTH);
      ErrnoCheck("Failed to write to socket", (rc < 0), rc);
      break;
    }
  }
  for ( i = 0; i < numClient; i++ ) {
    /* run handleClient concurrently */
    praseClient(clientPath[i]); // thread only handle client_id.in file
    messageHandler(i, clientfd);
  }

   bzero(message, MAX_LENGTH);
   sprintf(message, "%s%d%s%d%s%s","(", ENDOFREQUEST, COMMA, 0, COMMA, ")\0");
   rc = write(clientfd, message, MAX_LENGTH);
   ErrnoCheck("Failed to write to socket", (rc < 0), rc);
   close(clientfd);
}

/* handle message */
void messageHandler(int id, int clientfd) {
  int msgId = 0, index = 0, counter = 0;

  while (citys[index]) {
    bzero(message, MAX_LENGTH);
    sprintf(message,
            "%s%d%s%ld%s%s%s%s%s","(", CLIENTREQUEST, COMMA,
            strlen(citys[index]),
            COMMA, QUOTE ,citys[index], QUOTE,")\0");
    index ++;

    rc = write(clientfd, message, MAX_LENGTH);
    ErrnoCheck("Failed to write to socket", (rc < 0), rc);

    bzero(message, MAX_LENGTH);
    rc = read(clientfd, message, MAX_LENGTH);
    ErrnoCheck("Failed to read to socket", (rc < 0), rc);

    if (malformedMsgChecker()) {
      errorSet(clientfd);
    }

    msgId = atoi(message + 1);

    if (msgId == TRENDREPLY) {
      bzero(message, MAX_LENGTH);
      rc = read(clientfd, message, MAX_LENGTH);
      ErrnoCheck("Failed to read to socket", (rc < 0), rc);
      msgId = atoi(message + 1);
      if (msgId == ENDOFRESPONSE) {
        write2File( clientPath[id], citys[counter], trends );
      }
      counter++;
      bzero(message, MAX_LENGTH);
      freemakeargv(keywords);
    }

    if (msgId == ERROR) {
      bzero(message, MAX_LENGTH);
      fprintf(stderr, "Error happened on Server side\n");
      sprintf(message, "%s%d%s%d%s%s",
              "(", ERROR, COMMA, 0, COMMA, ")\0");
      break;
    }
  }
}

void errorSet(int clientfd){
  bzero(message, MAX_LENGTH);
  sprintf(message, "%s%d%s%d%s%s","(", ERROR, COMMA, payload, COMMA, ")\0");
  rc = write(clientfd, message, MAX_LENGTH);
  ErrnoCheck("Failed to write to socket", (rc < 0), rc);
  close(clientfd);
  exit(1);
}

int malformedMsgChecker() {
  string* msgvector;

  if (message[0] != '(') {
    return 1;
  }
  else if (message[strlen(message) - 1] != ')') {
    return 1;
  }
  else {
    if ((payload = atoi(message + 5))) {
      // test comma, shall be larger than 2
      if(makeargv(message, COMMA, &msgvector) < 2) {
        return 1;
      }
      else {
        makeargv(message, QUOTE, &keywords);
        if (payload != (int)strlen(keywords[1])) {
          return 1;
        }
        else {
          trends = keywords[1];
        }
        freemakeargv(msgvector);
      }
    }

  }
  return 0;
}


/* write result to result file */
void write2File( string path, string cityName, string trends ) {
  FILE* fp;
  string newPath = MALLOC(MAX_LENGTH, char);
  char toFile[MAX_LENGTH];

  sprintf(newPath,"%s%s", path, APPENDSTR);
  /* remove newline from newPath */

  if ((fp = fopen(newPath, "a")) == NULL) {
    perror("Failed to open file");
    exit(1);
  }

  sprintf(toFile, "%s : %s\n", cityName, trends);
  fwrite(toFile, sizeof(char), strlen(toFile), fp);

  if( ferror( fp ) != 0 )      { // error check
    perror( "I/O error when writing:" );
  }
}


/* extract client info */
void praseClient( string path ) {
  FILE* fp;
  int i = 0;
  string buffer = MALLOC(MAX_CITY, char);

  if ((fp = fopen(path, "r")) == NULL) {
    perror("Failed to open file");
  }
  while ( fgets(buffer, MAX_CITY, fp) ) {
    buffer[strlen(buffer) - 1] = NULLEND;
    citys[i] = CALLOC(MAX_BUF, char);
    strcpy(citys[i], buffer);
    i++;
  }
  citys[i] = NULL;
  free(buffer);
}

int main(int argc, char *argv[]) {
  int i, numClient;

  if (argc  <  4) {
    fprintf(stderr, "At least four arguments\n");
  }

  /* extract useful information from command line */
  numClient = argc - 3;
  hostname = argv[1];
  portNum = atoi(argv[2]);
  for (i = 0; i < numClient; i++) {
    clientPath[i] = argv[i + 3]; // argv[i] is file path
  }

  twitterTrendClient(numClient); // start to process clients

  return 0;
}
