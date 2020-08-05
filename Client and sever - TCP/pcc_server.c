#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>

unsigned int pcc_total[127];
unsigned int cur_total[127];
int stop;

void update_cur_total(char* buff, unsigned int N){
    int i;
    int cur_loc;
    for(i = 0; i < 127;i++){
        cur_total[i] = 0;
    }
    for(i = 0; i < N; i++){
        if(buff[i] >= 32 && buff[i] <= 126){
            cur_loc = buff[i];
            cur_total[cur_loc]++;
        }
    }
}

void update_pcc_total(){
    int i;
    for(i = 0; i < 127; i++){
        pcc_total[i] += cur_total[i];
    }
}

static void handler(int signum){
    if(stop == 0){
        stop=1;
    }
}

int main(int argc, char *argv[]) {
    unsigned int cur_total_sum;
    int listenfd = -1;
    int connfd = -1;
    int bites_written = 0;
    int cur_bites_written = 0;
    char *cur_buf;
    unsigned short server_port;
    unsigned int N;
    int i;
    stop = 0;
    struct sockaddr_in serv_addr;
    socklen_t addrsize = sizeof(struct sockaddr_in);
    struct sigaction sig;
    sig.sa_handler = handler;
    sig.sa_flags = SA_RESTART;
    memset(&serv_addr, 0, addrsize);
    for (i = 0; i < 127; i++) {
        pcc_total[i] = 0;
    }
    if (sigaction(SIGINT, &sig, NULL) != 0) {
        perror("sigaction failed\n");
        exit(1);
    }
    if (argc != 2) {
        perror("wrong no. of arguments");
        exit(1);
    }
    server_port = atoi(argv[1]);
    if (server_port == 0) {
        perror("invalid server port");
        exit(1);
    }
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("couldn't create listening socket");
        exit(1);
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
        perror("SO_REUSEADDR failed");
        exit(1);
    }
    if(fcntl(listenfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK)<0){
        perror("error in fcntl");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(server_port);
    if(0 != bind(listenfd,(struct sockaddr*) &serv_addr, addrsize)){
        perror("bind failed");
        exit(1);
    }
    if(0 != listen(listenfd, 10 )){
        perror("listen failed");
        exit(1);
    }
    start:
    while(stop == 0) {
        connfd = accept(listenfd, NULL, &addrsize);
        if (connfd < 0) {
            if (errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE && errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("accept failed");
                exit(1);
            }
            goto start;
        }
        bites_written = 0;
        while(bites_written < sizeof(unsigned int)) {
            if ((cur_bites_written = recv(connfd, &N, sizeof(unsigned int), 0)) < 0) {
                if (errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) {
                    perror("error while reading N from client");
                    exit(1);
                }
                goto start;
            }
            bites_written += cur_bites_written;
        }
        N = ntohl(N);
        cur_buf = (char*)malloc(sizeof(char)*N);
        bites_written = 0;
        while(bites_written < N) {
            if ((cur_bites_written = recv(connfd, (cur_buf) + bites_written, N-bites_written, 0)) < 0) {
                if (errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) {
                    perror("error while reading file from client");
                    exit(1);
                }
                goto start;
            }
            bites_written += cur_bites_written;
        }
        update_cur_total(cur_buf, N);
        free(cur_buf);
        cur_total_sum = 0;
        for(i = 32; i<=126; i++){
            cur_total_sum += cur_total[i];
        }
        cur_total_sum = htonl(cur_total_sum);
        bites_written = 0;
        while(bites_written < sizeof(cur_total_sum)) {
            if ((cur_bites_written = send(connfd, &cur_total_sum, sizeof(cur_total_sum), 0)) < 0) {
                if (errno != ETIMEDOUT && errno != ECONNRESET && errno != EPIPE) {
                    perror("error while sending C to client");
                    exit(1);
                }
                goto start;
            }
            bites_written += cur_bites_written;
        }
        update_pcc_total();
        close(connfd);
    }
    for(i=32; i<=126; i++){
        printf("char '%c' : %u times\n", i, pcc_total[i]);
    }
    close(listenfd);
    exit(0);
}

