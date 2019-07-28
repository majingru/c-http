#ifndef _STD_HTTP_CLIENT_H
#define _STD_HTTP_CLIENT_H

typedef struct {
    int port;
    const char * host;
    const char * schema;
} std_http_connection;

typedef struct {
    const char * method;
    const char * param;
    const char * uri;
    const char * header;
} std_http_request;

typedef struct{
    char * body;
    char * status;
    char * header;
} std_http_response;

std_http_connection * std_http_connect(const char * host, int port);

void std_free_connection(std_http_connection * connection);

std_http_request * std_make_request(const char * uri, const char * param, const char * custom_header);

void std_free_request(std_http_request * request);

std_http_response * std_http_get(std_http_connection * connection, std_http_request *request);
std_http_response * std_http_post(std_http_connection * connection, std_http_request *request, const char * post_data);
std_http_response * std_http_head(std_http_connection * connection, std_http_request *request);
std_http_response * std_http_option(std_http_connection * connection, std_http_request *request);

void std_free_response(std_http_response * response);
#endif

