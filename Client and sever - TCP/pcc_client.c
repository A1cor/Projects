#define _DEFAULT_SOURCE

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>




struct FileBuffer{
    char* buff;
    unsigned int size;
};

void read_file(FILE *fptr,  struct FileBuffer *ret){
    size_t size;
    fseek(fptr, 0L, SEEK_END);
    ret->size = ftell(fptr) + 1;
    fseek(fptr, 0L, SEEK_SET);
    ret->buff = (char*)malloc(sizeof(char)*ret->size);
    memset(ret->buff, 0, ret->size);
    size = fread(ret->buff, ret->size, 1, fptr);
    if(size != 0){
        perror("error while reading the file");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    FILE *fptr;
    int sockfd = -1;
    unsigned int num_of_valid_chars;
    int bites_written = 0;
    int cur_bites_written = 0;
    struct FileBuffer buf;
    struct FileBuffer *cur_buf = &buf;
    unsigned short server_port;
    unsigned int N;
    struct sockaddr_in serv_addr;
    socklen_t addrsize = sizeof(struct sockaddr_in );
    memset(&serv_addr, 0, addrsize);
    if(argc != 4){
        printf("wrong no. of arguments");
        exit(1);
    }
    if((inet_aton(argv[1],&serv_addr.sin_addr)) == 0){
        perror("invalid address");
        exit(1);
    }
    server_port = atoi(argv[2]);
    if(server_port == 0){
        perror("invalid server port");
        exit(1);
    }
    fptr = fopen(argv[3],"r");
    if(fptr == NULL){
        perror("file doesn't exist");
        exit(1);
    }
    read_file(fptr, cur_buf);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("couldn't create socket");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    if(connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("connection failed");
        exit(1);
    }
    N = htonl(cur_buf->size);
    while(bites_written < sizeof(cur_buf->size)) {
        if ((cur_bites_written = write(sockfd, &N, sizeof(cur_buf->size))) < 0) {
            perror("error while sending N to server");
            exit(1);
        }
        bites_written += cur_bites_written;
    }
    bites_written = 0;
    while(bites_written < cur_buf->size) {
        if ((cur_bites_written = write(sockfd, (cur_buf->buff) + bites_written, cur_buf->size-bites_written)) < 0) {
            perror("error while writing file to server");
            exit(1);
        }
        bites_written += cur_bites_written;
    }
    bites_written = 0;
    while(bites_written < sizeof(unsigned int)) {
        if ((cur_bites_written = read(sockfd, &num_of_valid_chars, sizeof(unsigned int))) < 0) {
            perror("error while reading C from server");
            exit(1);
        }
        bites_written += cur_bites_written;
    }
    num_of_valid_chars = ntohl(num_of_valid_chars);
    printf("# of printable characters: %u\n", num_of_valid_chars);
    fclose(fptr);
    close(sockfd);
    free(cur_buf->buff);
    exit(0);
}
