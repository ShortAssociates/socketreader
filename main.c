/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Paul Short 2010 <paul@short-associates.com>
 * 
 */

#include <stdio.h>      
#include <sys/socket.h> // for socket(), connect(), sendto(), and recvfrom()
#include <sys/poll.h>   // for poll() to get socket status
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     
#include <string.h> 
#include <ctype.h>    
#include <unistd.h>     // for close() 
#include <pthread.h>    // POSIX multithreading libary
#include <sched.h>		// Needed to change scheduling priority on threads
#include <stdarg.h>
#include <sys/time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>        /*  socket types              */

//#include "MDSClient.h"

#ifndef PG_SOCK_HELP
#define PG_SOCK_HELP


#include <unistd.h>             /*  for ssize_t data type  */

#define LISTENQ        (1024)   /*  Backlog for listen()   */
#define MDS_INPUT_BUFFER	64

#define SOCKET_SERVER_IP		"192.168.224.1"
#define SOCKET_SERVER_PORT		5025

/*  Function declarations  */

ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Writeline(int fc, const void *vptr, size_t maxlen);


#endif  /*  PG_SOCK_HELP  */



/*  Global constants  */

#define MAX_LINE           (1000)
#define BUFFER_SIZE	1024


// ****************************************************************************
//		Error Handling
//			This is the error and debugging subsystem
//			It runs in its own thread with a lower priority
// ****************************************************************************
inline void fnDebug( char *szMsg )
{
	struct timeval tv;
	char szErrorBuffer[BUFFER_SIZE];

	gettimeofday( &tv, NULL );
	snprintf( szErrorBuffer, sizeof(szErrorBuffer),"Debug: %i:%06i : %s", (int)tv.tv_sec, (int)tv.tv_usec, szMsg );
	printf( "%s\n", szErrorBuffer );

	//pthread_mutex_lock( &qErrorMessages_mutex );
	//fnErrPush( &qErrorMessages, szErrorBuffer );
	//pthread_mutex_unlock( &qErrorMessages_mutex );
}       

// Read a line from a socket
ssize_t Readline(int sockd, void *vptr, size_t maxlen) 
{
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) 
	{
	
		if ( (rc = read(sockd, &c, 1)) == 1 ) 
		{
	    	*buffer++ = c;
	    	if ( c == '\n' )
				break;
		}
		else if ( rc == 0 ) 
		{
	    	if ( n == 1 )
				return 0;
	    	else
				break;
		}
		else 
		{
	    	if ( errno == EINTR )
				continue;
	    	return -1;
		}
    }

    *buffer = 0;
    return rc;
}




//  main()  
int main(int argc, char *argv[]) {

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress;             /*  Holds remote IP address   */
    char     *szPort;                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */
	char	 *szTradeDate;
	char	 *szWeightFile;
    time_t 		tTime;
    double		dStart, dEnd;
    struct tm 	*now;
    char 		szLineBuffer[2048];
    struct timeval tvCurrentTime, tvEndTime;
    int 		iReturn, iHB;
    double 		dMilliseconds;
    long 		lLines = 0;
    long 		iCounter=0;
    long 		iThousands=0;
    long 		lSkipped = 0;




    //  Get command line arguments
    // ParseCmdLine(argc, argv, &szAddress, &szPort, &szTradeDate, &szWeightFile);
	//fnLoadWeights( "NASDAQ100.txt", szTradeDate );


    //  Set the remote port
    port = SOCKET_SERVER_PORT;
	

    //  Create the listening socket
    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{
		fprintf(stderr, "MDSClient: Error creating listening socket.\n");
		exit(EXIT_FAILURE);
    }


    // Set all bytes in socket address structure to zero
	// fill in the relevant data members
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    //  Set the remote IP address
    if ( inet_aton(SOCKET_SERVER_IP, &servaddr.sin_addr) <= 0 ) 
	{
		printf("ECHOCLNT: Invalid remote IP address.\n");
		exit(EXIT_FAILURE);
    }

    
    //  connect() to the remote server
    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) 
	{
		printf("ECHOCLNT: Error calling connect()\n");
		exit(EXIT_FAILURE);
    }

    // Get the current time in Milliseconds
    iReturn = gettimeofday(&tvCurrentTime, NULL);
    dMilliseconds = (double)tvCurrentTime.tv_sec*1000000.0 + (double)tvCurrentTime.tv_usec;
    dStart = (double)tvCurrentTime.tv_sec + (double)(tvCurrentTime.tv_usec/1000000.0);
    printf( "Starting Processing\n");

	iHB=0;

	for( iCounter=0; iCounter<1000000000; iCounter++ )
	{
		memset(szLineBuffer, 0, sizeof(szLineBuffer) );
	    Readline(conn_s, szLineBuffer, sizeof(szLineBuffer) );
		fnDebug( szLineBuffer );
	}
	return 1;
}