#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 5000
#define MAX_MSG_SIZE 4096

static char expected_req[] = "POST /api/echo HTTP/1.1\r\n";
static char resp_template[] = "HTTP/1.1 200 OK\r\n"
                              "Server: Custom/1.0.0 Custom/1.0.0\r\n"
                              "Date: %s\r\n" /* current datetime */
                              "Content-Length: %lu\r\n" /* body length */
                              /* TODO:  what to put here? */
                              "Connection: close\r\n"
                               "\r\n"
                              "%s"; /* body */

int main()
{
    int rc;
    int lsock, sock;
    ssize_t len;
    char msg[MAX_MSG_SIZE], resp[MAX_MSG_SIZE];

    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) {
        fprintf(stderr, "Error creating socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int v = 1;
    if (setsockopt(lsock, SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v))) {
        fprintf(stderr, "Unable to set SO_REUSEPORT sockopt\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(lsock, (struct sockaddr *)&addr, sizeof(addr))) {
        fprintf(stderr, "Error binding to port %d: %s\n", PORT,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(lsock, 10)) {
        fprintf(stderr, "Error listening: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("[RC] Server listening on %d\n", PORT);

    for (;;) {
        sock = accept(lsock, NULL, NULL);
        if (sock < 0) {
            fprintf(stderr, "Error accepting connection: %s\n",
                    strerror(errno));
            exit(EXIT_FAILURE);
        }

        len = recv(sock, msg, MAX_MSG_SIZE, 0);
        if (len <= 0) {
            fprintf(stderr, "Error receiving message: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        msg[len] = 0;

        /* Check the header is good */
        if (strncmp(msg, expected_req, sizeof(expected_req) - 1)) {
            fprintf(stderr, "Received unexpected message:\n%s\n", msg);
            exit(EXIT_FAILURE);
        }

        /* Locate body */
        char *body = strstr(msg, "\r\n\r\n");
        if (!body) {
            fprintf(stderr, "Received request without body:\n%s", msg);
            exit(EXIT_FAILURE);
        }
        body += 4; /* Skip newlines */

        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char timestr[64];
        if (!strftime(timestr, sizeof(timestr), "%c", tm)) {
            fprintf(stderr, "Error getting time string: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        sprintf(resp, resp_template, timestr, strlen(body), body);
        if (send(sock, resp, strlen(resp), 0) != strlen(resp)) {
            fprintf(stderr, "Error sending message: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        close(sock);
    }

    close(lsock);

    return 0;
}