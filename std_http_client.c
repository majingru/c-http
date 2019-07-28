#include "std_http_client.h"
#include "std_string.h"
#include "std_log.h"
#ifdef STD_OS_WIN
#include <winsock.h>
#else
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

static bool wsas_inited = false;

static void init_wsas()
{
#ifdef STD_OS_WIN
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) == SOCKET_ERROR)
    {
        log_error("WSAStartup() fail\n");
    }
#endif
}

static void destroy_wsas()
{
#ifdef STD_OS_WIN
    if (wsas_inited)
    {
        WSACleanup();
        wsas_inited = false;
    }
#endif
}

static void close_sock(int sock)
{
#ifdef STD_OS_WIN
    closesocket(sock);
#else
    close(sock);
#endif
}

static int make_connect(const char *host, int port)
{
    int sockfd;
    struct sockaddr_in serveraddr;


    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        log_error("Can't create TCP socket \n");
        return 0;
    }

    struct hostent *hent;

    if ((hent = gethostbyname(host)) == NULL)
    {
        log_error("can't get ip\n");
        return 0;
    }

    char *ip = inet_ntoa(*(struct in_addr *)*hent->h_addr_list);

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        close_sock(sockfd);
        log_error("connect() fail\n");
        return 0;
    }

    return sockfd;
}

std_http_connection *std_http_connect(const char *host, int port)
{
    if (host == NULL || port == 0)
    {
        return NULL;
    }

    if (wsas_inited == false)
    {
        init_wsas();
        wsas_inited = true;
    }
    
    std_http_connection *conn = (std_http_connection*)std_malloc(sizeof(std_http_connection));
    if (conn == NULL)
    {
        return NULL;
    }
    conn->host = host;
    conn->port = port;
    conn->schema = "http";
    return conn;
}

void std_free_connection(std_http_connection *conn)
{
    if (conn == NULL)
    {
        return;
    }
    destroy_wsas();
    conn->host = NULL;
    conn->port = 0;
    std_free(conn);
}

std_http_request *std_make_request(const char *uri, const char *param, const char *custom_header)
{
    std_http_request *req = (std_http_request*)std_malloc(sizeof(std_http_request));
    if (req == NULL)
    {
        log_error("malloc error when create http request");
        return NULL;
    }

    req->header = custom_header;
    req->uri = uri;
    req->param = param;
    return req;
}

void std_free_request(std_http_request *request)
{
    if (request == NULL)
    {
        log_error("free request failed. request is null.");
        return;
    }

    request->header = NULL;
    request->param = NULL;
    request->uri = NULL;
    std_free(request);
}

static std_http_response *http_send(std_http_connection *conn, char *request)
{
    log_debug("http request: %s", request);
    int len = strlen(request);
    int sock = make_connect(conn->host, conn->port);
    if (sock == 0)
    {
        return NULL;
    }
    int sent = send(sock, request, len, 0);

    if (sent != len)
    {
        close_sock(sock);
        log_error("Can't send query");
        return NULL;
    }

    int tmpres = 0;
    char buf[BUFSIZ];

    String resp = std_string_malloc(BUFSIZ);
   // while ((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0)
   // {
   //    std_string_appends(&resp, buf, tmpres);
   //     memset(buf, 0, BUFSIZ);
   //      if (tmpres < BUFSIZ)
   //     {
   //          break;
   //     }
   // }
    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    while ((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0)
    {
        std_string_appends(&resp, buf, tmpres);
        memset(buf, 0, BUFSIZ);
        if (tmpres < BUFSIZ)
        {
            break;
        }
    }
    close_sock(sock);
    int s_index = std_string_index(&resp, "\r\n");
    char *status = std_string_sub(&resp, 8, s_index);
    log_debug("http response status: %s", status);
    int h_index = std_string_index(&resp, "\r\n\r\n");
    char *header = std_string_sub(&resp, s_index + 2, h_index);
    log_debug("http response header: %s", header);
    char *body = std_string_sub(&resp, h_index + 4, resp.used);
    log_debug("http response body: %s", body);
    std_string_free(resp);

    std_http_response *s_resp = (std_http_response*)std_malloc(sizeof(std_http_response));
    s_resp->body = body;
    s_resp->header = header;
    s_resp->status = status;
    return s_resp;
}

static char *make_request_string(std_http_connection *conn, std_http_request *request)
{
    String buff = std_string_malloc(BUFSIZ);

    const char *uri = "/";
    if (request->uri)
    {
        uri = request->uri;
    }

    const char *param = request->param;

    if (param == NULL)
    {
        std_string_appendf(&buff, "%s %s HTTP/1.1\r\nUser-Agent: simple http client in c\r\nHost:%s:%d\r\nConnection: Keep-Alive\r\nContent-Length:0\r\n", request->method, uri, conn->host, conn->port);
    }
    else
    {
        std_string_appendf(&buff, "%s %s?%s HTTP/1.1\r\nUser-Agent: simple http client in c\r\nHost:%s:%d\r\nConnection: Keep-Alive\r\nContent-Length:0\r\n", request->method, uri, param, conn->host, conn->port);
    }

    if (request->header)
    {
        std_string_appendf(&buff, "%s\r\n", request->header);
    }
    else
    {
        std_string_appends(&buff, "\r\n", strlen("\r\n"));
    }

    char *s_req = std_string_to_s(buff);
    std_string_free(buff);
    return s_req;
}

static char * make_get_string(const char * hostname,const char * path, const char * param)
{
     char *request = NULL;
    String buff = std_string_malloc(BUFSIZ);

    std_string_appendf(&buff, "GET %s?%s HTTP/1.0\r\n", path, param);
    std_string_appendf(&buff, "Host: %s\r\n", hostname);
    std_string_appendf(&buff, "Connection: close\r\n\r\n");

    request = std_string_to_s(buff);
    std_string_free(buff);

    return request;
}

std_http_response *std_http_get(std_http_connection *conn, std_http_request *request)
{
    if (conn == NULL || request == NULL)
    {
        log_error("Parameter Error. connection is null or request is null.");
        return NULL;
    }

    request->method = "GET";
    char *s_req = make_get_string(conn->host, request->uri, request->param);

    std_http_response *s_resp = http_send(conn, s_req);
    std_free(s_req);
    return s_resp;
}

std_http_response *std_http_post(std_http_connection *conn, std_http_request *request, const char *post_data)
{
    if (conn == NULL || request == NULL || post_data == NULL)
    {
        log_error("Parameter Error. connection is null or request or post_data is null.");
        return NULL;
    }

    String buff = std_string_malloc(BUFSIZ);

    const char *uri = "/";
    if (request->uri)
    {
        uri = request->uri;
    }
    const char *param = request->param;

    if (param == NULL)
    {
        std_string_appendf(&buff, "POST %s HTTP/1.1\r\nHost:%s:%d\r\nUser-Agent: simple http client in c\r\nConnection: Keep-Alive\r\nContent-Length:%d\r\nContent-Type:application/json\r\n", uri, conn->host, conn->port, strlen(post_data));
    }
    else
    {
        std_string_appendf(&buff, "POST %s?%s HTTP/1.1\r\nHost:%s:%d\r\nUser-Agent: simple http client in c\r\nConnection: Keep-Alive\r\nContent-Length:%d\r\nContent-Type:application/json\r\n", uri, request->param, conn->host, conn->port, strlen(post_data));
    }

    if (request->header)
    {
        std_string_appendf(&buff, "%s\r\n%s", request->header, post_data);
    }
    else
    {
        std_string_appendf(&buff, "\r\n%s", post_data);
    }

    char *s_req = std_string_to_s(buff);
    std_string_free(buff);
    std_http_response *s_resp = http_send(conn, s_req);
    std_free(s_req);
    return s_resp;
}

std_http_response *std_http_head(std_http_connection *conn, std_http_request *request)
{
    if (conn == NULL || request == NULL)
    {
        log_error("Parameter Error. connection is null or request is null.");
        return NULL;
    }

    request->method = "HEAD";
    char *s_req = make_request_string(conn, request);
    std_http_response *s_resp = http_send(conn, s_req);
    std_free(s_req);
    return s_resp;
}
std_http_response *std_http_option(std_http_connection *conn, std_http_request *request)
{
    if (conn == NULL || request == NULL)
    {
        log_error("Parameter Error. connection is null or request is null.");
        return NULL;
    }

    request->method = "OPTION";

    char *s_req = make_request_string(conn, request);
    std_http_response *s_resp = http_send(conn, s_req);
    std_free(s_req);
    return s_resp;
}

void std_free_response(std_http_response *resp)
{
    if (resp)
    {
        std_free(resp->body);
        std_free(resp->header);
        std_free(resp->status);
        std_free(resp);
    }
}
