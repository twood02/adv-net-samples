nclude <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

int main(int argc, char ** argv)
{
	//port used to connect to socket
//	char* server_port = "80";
	char * server_port = "1234";
//	char* server_port= "1234";
	//ip address of server http request will be sent to
	//char* server_ip = "173.194.121.36";
	char* server_ip = "127.0.0.1";
	//char* server_ip = "128.164.144.179";
	//char * hostName = "http://www.cs.gwu.edu";
	//char * host_name = "http://www.google.com";
	char * host_name = "http://localhost:1234/hello.html";
	//socket int used to connect
	int sockfd, rc;
	//used to store formatted http request
	char * http_format_req; 
	struct addrinfo hints, *server;
	char *message = "";
	//bytes received from server
	int bytes_received = -1;
	//data received from http request--will be printed out to user
	char * recv_data = (char *)malloc(sizeof(char*)*1000);
	int o;

        /* Command line args:
                -p port
                -h host name or IP
        */
        while ((o = getopt (argc, argv, "p:h:m:i:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        host_name = optarg;
                        break;
                case 'm':
                        message = optarg;
                        break;
		case 'i':
			server_ip = optarg;
			break;
                case '?':
                        if(optopt == 'p' || optopt == 'h' || optopt == 'i' ) {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }
	
	//mallocs memory that will be used for http request.
	http_format_req = (char*)malloc(sizeof(char*)*(strlen("GET / HTTP/1.1\r\r")+strlen(host_name) + strlen(message)));

	//formats the http request
	sprintf(http_format_req,"GET /%s HTTP/1.0\n\n", message);
	printf("%s\n", http_format_req);
	/* The hints struct is used to specify what kind of server info we are looking for */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */

	/* getaddrinfo() gives us back a server address we can connect to.
        It actually gives us a linked list of addresses, but we'll just use the first.
	 */
	if ((rc = getaddrinfo(server_ip, server_port, &hints, &server)) != 0) {
		perror(gai_strerror(rc));
		exit(-1);
	}

	/* Now we can create the socket and connect */
	sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

	if (sockfd == -1) {
		perror("ERROR opening socket");
		exit(-1);
	}

	rc = connect(sockfd, server->ai_addr, server->ai_addrlen);
	if (rc == -1) {
		perror("ERROR on connect");
		close(sockfd);
		exit(-1);
		// TODO: could use goto here for error cleanup
	}

	/* Sends the http request. */
	rc = send(sockfd,http_format_req,strlen(http_format_req), 0);
	if(rc < 0) {
		perror("ERROR on send");
		exit(-1);
	}
	//default buffer size is 1024.  recv receives the info from the server.
	bytes_received = recv(sockfd,recv_data,1024,0);

	while(bytes_received)
	{
		//default buffer size is 1024
		bytes_received = recv(sockfd,recv_data,1024,0);

		if(bytes_received == -1)
		{
			perror("recv");
			exit(1);
		}

		//prints out information to user
		printf("%s\n",recv_data);
		if(bytes_received==0){ 
			break; 
		}
		recv_data[bytes_received] = '\0';
	}

	//closes socket
	out:
		freeaddrinfo(server);
	close(sockfd);

	printf("Done.\n");
	return 0;

}

