#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"httplib.h"
#include<sys/stat.h>

void str_slice (char* dst_string, char* src_string)
{
	char tmp_string[64] = {0};
	int counter = 0;
	int tmp_counter = 0;
	int flag = 1;
	int compare_condition = 1;
	while(flag)
	{
		if((dst_string[counter] == src_string[counter]) && compare_condition){
			// Deleting the whitespace from the array, if u want to delete slash as well increment by 3
			counter += 2;
		}else{
			if(counter==64){
				flag = 0;
			}else if(counter<64 && compare_condition){
				compare_condition = 0;
				tmp_string[tmp_counter] = dst_string[counter];
				counter++;
				tmp_counter++;
			}else{
				tmp_string[tmp_counter] = dst_string[counter];
				counter++;
				tmp_counter++;
			}
		}
	}
	strcpy(dst_string,tmp_string);
}

int serve_html(char* filename, int socket_des)
{
	char buffer[2100];
	int fd = open((const char*)filename,O_RDONLY);
	if(fd == -1)
	{
		perror("file open err");
		return -1;
	}
	if(read(fd,&buffer,sizeof(buffer)) == -1 )
	{
		perror("read serve_html");
		return -1;
	}
	if(write(socket_des,buffer,sizeof(buffer)) == -1 )
	{
		perror("write serve_html");
		return -1;
	}
	if(close(fd) == -1)
	{
		perror("serve_html des close");
		return -1;
	}
	return 1;
}
