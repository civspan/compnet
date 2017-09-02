/********************************************************* -- SOURCE -{{{1- */
/** Translate host name into IPv4
 *
 * Resolve IPv4 address for a given host name. The host name is specified as
 * the first command line argument to the program. 
 *
 * Build program:
 *  $ gcc -Wall -g -o resolve <file>.cpp
 */
/******************************************************************* -}}}1- */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>


//--//////////////////////////////////////////////////////////////////////////
//--    local declarations          ///{{{1///////////////////////////////////

void print_usage( const char* aProgramName );

//--    local config                ///{{{1///////////////////////////////////

/* HOST_NAME_MAX may be missing, e.g. if you're running this on an MacOS X
 * machine. In that case, use MAXHOSTNAMELEN from <sys/param.h>. Otherwise 
 * generate an compiler error.
 */
#if !defined(HOST_NAME_MAX)
#	if defined(__APPLE__)
#		include <sys/param.h>
#		define HOST_NAME_MAX MAXHOSTNAMELEN
#	else  // !__APPLE__
#		error "HOST_NAME_MAX undefined!"
#	endif // ~ __APPLE__
#endif // ~ HOST_NAME_MAX

//--    main()                      ///{{{1///////////////////////////////////
int main( int aArgc, char* aArgv[] )
{
	// Check if the user supplied a command line argument.
	if( aArgc != 2 )
	{
		print_usage( aArgv[0] );
		return 1;
	}

	// The (only) argument is the remote host that we should resolve.
	const char* remoteHostName = aArgv[1];

	// Get the local host's name (i.e. the machine that the program is 
	// currently running on).
	const size_t kHostNameMaxLength = HOST_NAME_MAX+1;
	char localHostName[kHostNameMaxLength];

	if( -1 == gethostname( localHostName, kHostNameMaxLength ) )
	{
		perror( "gethostname(): " );
		return 1;
	}

	// Print the initial message
	printf( "Resolving `%s' from `%s':\n", remoteHostName, localHostName );

/////// TODO : add your code here /////////////////////////////////////////////
	struct addrinfo *result;
	struct addrinfo hints;
	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;    /* Allow IPv4 */
        hints.ai_socktype =  SOCK_STREAM; /* TCP only */
        hints.ai_flags = 0;
        hints.ai_protocol = IPPROTO_TCP;          /* TCP only */
	hints.ai_canonname = 0;
	hints.ai_addr = 0;
	hints.ai_next = 0;

	int s = getaddrinfo(remoteHostName, NULL, &hints, &result);
	if (s != 0) { /*error handling */
	  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	  exit(EXIT_FAILURE);
	}
	sockaddr* sockAddr = result->ai_addr; /*get pointer to sockaddr from result struct*/
	assert( AF_INET == sockAddr->sa_family ); /*test to see sa_family is IPv4*/
	sockaddr_in* inAddr = (sockaddr_in*)sockAddr; /*cast to sockaddr_in*/
	//	int port = inAddr->sin_port; /*get port..*/
	uint32_t ipNumber = inAddr->sin_addr.s_addr /*.. and ip*/;

	char addr_string[INET_ADDRSTRLEN]; /*create string of length of ipv4*/
	if(inet_ntop(AF_INET, &ipNumber, addr_string, INET_ADDRSTRLEN) != NULL)
	  /* if possible to convert ip address to string, to so and print */
	  printf("%s has address %s\n", remoteHostName, addr_string);
	else {
	  /* if not possible to convert, print error message and quit */
	  printf("Quitting, error: %s\n", strerror(errno));
	  exit(-1);
	}
	freeaddrinfo(result);           /* Free mem */	

	
	
/////// END OF TODO ///////////////////////////////////////////////////////////	
	// Ok, we're done. Return success.
	return 0;
}


//--    print_usage()               ///{{{1///////////////////////////////////
void print_usage( const char* aProgramName )
{
	fprintf( stderr, "Usage: %s <hostname>\n", aProgramName );
}

//--///}}}1/////////////// vim:syntax=cpp:foldmethod=marker:ts=4:noexpandtab: 
