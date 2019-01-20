#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<netdb.h>
#include"httplib.h"
#include"addon.c"

typedef struct{
	char address[3][64];
} routes;

typedef struct{
	char method[64];
	char route[64];
} http_method;

static const http_method RESET_HTTP_STRUCT;

http_method get_route(char *http_fline)
{
	http_method tmp_struct;
	char tmp_buffer[64] = {0};
	int flag = 0;
	int counter = 0;
	int tmp_counter = 0;
	while(flag!=2 && counter<64)
	{
		if(http_fline[counter]==' ')
		{
			if(flag<1){
				flag = 1;
				strcpy(tmp_struct.method,tmp_buffer);
			}else{
				flag = 2;
				strcpy(tmp_struct.route, tmp_buffer);
				//it cuts the GET /
				str_slice(tmp_struct.route, tmp_struct.method);
			}
		}
		tmp_buffer[counter] = http_fline[counter];
		counter += 1;
	}
	return tmp_struct;
}

void http_header_getfline(char *buff,char *header_route)
{
	int flag = 0;
	int counter = 0;
	while(flag!=1)
	{
		if(buff[counter] == '\r')
		{
			flag = 1;
			header_route[counter] = buff[counter];
		}else{
			header_route[counter] = buff[counter];
			counter++;
		}
	}
}

int is_route_valid(char *route, char struct_routes[3][64])
{
	int counter = 0;
	while(counter < 3)
	{
		if(strcmp(route,struct_routes[counter]) == 0 )
		{
			return 1;
		}else{
			counter++;
		}
	}
	return -1;
}

void response(char *route, int socket_des)
{
	if(strcmp(route,"/") == 0){
		if(serve_html("index.html",socket_des) == -1)
				perror("serve_html");
	}else if(strcmp(route,"/index") == 0){
		if(serve_html("index.html",socket_des) == -1)
				perror("serve_html");
	}else if(strcmp(route,"/easteregg") == 0){
		if(serve_html("index.html",socket_des) == -1)
				perror("serve_html");
	}
}

int main(void){
	//All variables :
	//struct from the socket address_in
	struct sockaddr_in server_address;
	//First main socket
	int socket_fd;
	//That is the flag for reusing the address
	int option = 1;
	//Socket created for the communication per request
	int reply_socket_fd;
	//Buffer for the whole req
	char buffer[1024] = {0};
	// buffer for the res
	char res_msg[128];
	//Check if data was send from the client
	int read_value;
	//Route demanded from the client fline
	char header_route[64] = {0};
	//Struct for http header
	http_method http_str;
	//Routes provided by the server
	routes server_routes;
	strcpy(server_routes.address[0],"/");
	strcpy(server_routes.address[1],"/index");
	strcpy(server_routes.address[2],"/easteregg");
	//Flag for opening the connection
	int server_alive = 1;

	if((socket_fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket create err");
		exit(EXIT_FAILURE);
	}

	int address_reset = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if(address_reset == -1)
	{
		perror("setsockopt error");
		exit(EXIT_FAILURE);
	}

	memset(&server_address,'0',sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int addrlen = sizeof(server_address);

	if(bind(socket_fd,(struct sockaddr*)&server_address,addrlen)<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if(listen(socket_fd,user_queue)==-1)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	//for accept addrlen is equal to the size of the struct servaddr_in
	while(server_alive){

		http_str = RESET_HTTP_STRUCT;
		bzero(header_route,64);
		bzero(buffer,1024);
		bzero(res_msg,2048);

		printf("\n<<<<<<<<< WAITING FOR THE CONNECTION >>>>>>>>>>>\n");

		reply_socket_fd = accept(socket_fd,(struct sockaddr *)&server_address,(socklen_t*)&addrlen);
		if(reply_socket_fd == -1)
		{
			perror("accept failed");
			exit(EXIT_FAILURE);
		}

		read_value = read(reply_socket_fd,&buffer,1024);

		http_header_getfline(buffer,header_route);

		http_str = get_route(header_route);

		printf("%s\n",http_str.method);
		printf("%s\n",http_str.route);

		if(is_route_valid(http_str.route, server_routes.address) == -1)
		{
			strcpy(res_msg,"HTTP/1.1 404 Not Found");
		}else{
			response(http_str.route, reply_socket_fd);
		}

		//funnynote: if you dont write zero-valued bytes u will
		//constantly write over previous data
		if(read_value==-1)
		{
			puts("no bytes were there to read");
		}
		//write(reply_socket_fd,res_msg,strlen(res_msg));

		printf("\n<<<<<<<<< MESSAGE SENT >>>>>>>>>>>\n");
		close(reply_socket_fd);
	}

	return 0;
}
