//////////////////////////////////////////////////////////////////////////////////////
// Mail: radiotail86@gmail.com
// About the details of license, please read LICENSE
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "http_packer.h"

enum http_status_class {
    Informational = 1,
    Success,
    Redirection,
    Client_Error,
    Server_Error,
};

static char* http_status_informational[] = {
    "Continue",
    "Switching Protocols",
    "Processing",
};

static char* http_status_success[] = {
    "OK",
	"Created",
	"Accepted",
	"Non-Authoritative Information",
	"No Content",
	"Reset Content",
	"Partial Content",
	"Multi-Status",
	"Already Reported",
	"IM Used",
};

static char* http_status_redirection[] = {
    "Multiple Choices",
	"Moved Permanently",
	"Found",
	"See Other",
	"Not Modified",
	"Use Proxy",
	"Temporary Redirect",
	"Permanent Redirect",
};

static char* http_status_client_error[] = {
    "Bad Request",
	"Unauthorized",
	"Payment Required",
	"Forbidden",
	"Not Found",
	"Method Not Allowed",
	"Not Acceptable",
	"Proxy Authentication Required",
	"Request Time-out",
	"Conflict",
	"Gone",
	"Length Required",
	"Precondition Failed",
	"Request Entity Too Large",
	"Request-URI Too Large",
	"Unsupported Media Type",
	"Requested range not satisfiable",
	"Expectation Failed",
	"Payload Too Large",
	"URI Too Long",
	"Unsupported Media Type",
	"Range Not Satisfiable",
    "Expectation Failed",
    "I'm a teapot",
};

static char* http_status_server_error[] = {
    "Internal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Service Unavailable",
	"Gateway Time-out",
	"HTTP Version not supported",
	"Variant Also Negotiates",
    "Insufficient Storage",
    "Loop Detected",
    "Not Extended",
    "Network Authentication Required",
};

#define http_status_array_size(array) sizeof(array)/sizeof(char*)

static const char* http_status_desc(int status) {
    int class;
    int index;

    class = status / 100;
    index = status - class * 100;

    switch (class) {
    case Informational:
		if (http_status_array_size(http_status_informational) < index) {
            return NULL;
        }

        return http_status_informational[index];
    case Success:
		if (http_status_array_size(http_status_success) < index) {
            return NULL;
        }

        return http_status_success[index];
    case Redirection:
		if (http_status_array_size(http_status_redirection) < index) {
            return NULL;
        }

        return http_status_redirection[index];
    case Client_Error:
		if (http_status_array_size(http_status_client_error) < index) {
            return NULL;
        }

        return http_status_client_error[index];
    case Server_Error:
		if (http_status_array_size(http_status_server_error) < index) {
            return NULL;
        }

        return http_status_server_error[index];
    default:
        assert("error status!");
    }

    return NULL;
}


static __inline unsigned http_nextPower2(int n) {
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n++;
	return n;
}

int http_packer_init(struct http_packer* packer, enum http_packer_type type) {
    assert(packer);

    packer->buf = packer->fix_buf;
    packer->size = HEADER_FIX_BUF;
	packer->len = 0;
    packer->type = type;

    return 0;
}

int http_packer_reset(struct http_packer* packer, enum http_packer_type type) {
	assert(packer);

	packer->len = 0;
	packer->type = type;

	return 0;
}

int http_packer_destroy(struct http_packer* packer) {
	assert(packer);

	if (packer->buf != packer->fix_buf) {
		free(packer->buf);
	}

	return 0;
}

static void http_packer_resize(struct http_packer* packer, size_t need) {
    if (need > packer->size) {
        packer->size = http_nextPower2(need);
        if (packer->buf == packer->fix_buf) {
            packer->buf = (char*)malloc(packer->size);
            memcpy(packer->buf, packer->fix_buf, sizeof(packer->fix_buf));
        } else {
            packer->buf = (char*)realloc(packer->buf, packer->size);
        }
    }
}

int http_packer_header(struct http_packer* packer, const char* field, const char* value) {
    size_t need;
    size_t field_size;
    size_t value_size;

    if (!field || !value) {
        return -1;
    }

    field_size = strlen(field);
    value_size = strlen(value);
	need = packer->len + field_size + value_size + 5;
    http_packer_resize(packer, need);

	sprintf(packer->buf + packer->len, "%s: %s\r\n", field, value);
	packer->len = need - 1;

    return 0;
}

int http_packer_request_line(struct http_packer* packer, const char* method, const char* host, const char* url, int port) {
    size_t need;
    size_t method_size;
    size_t url_size;
	char* url_value;
	char host_value[128];

    if (!method || !host || !url) {
        return -1;
    }

    if (packer->type != HTTP_PACKER_REQUEST) {
        return -1;
    }

	url_value = (char*)url;
	if (strcmp(url, "") == 0) {
		url_value = "/";
	}

    method_size = strlen(method);
	url_size = strlen(url_value);
	need = packer->len + method_size + url_size + 13;
    http_packer_resize(packer, need);

	sprintf(packer->buf + packer->len, "%s %s HTTP/1.1\r\n", method, url_value);
	packer->len = need - 1;

	if (port == 80) {
		http_packer_header(packer, "Host", host);
	} else {
		sprintf(host_value, "%s:%d", host, port);
		http_packer_header(packer, "Host", host_value);
	}

    return 0;
}

int http_packer_respone_line(struct http_packer* packer, int status, const char* desc) {
    size_t need;
    const char* status_desc;

    if (packer->type != HTTP_PACKER_RESPONSE) {
        return -1;
    }

    status_desc = http_status_desc(status);
    if (status_desc) {
        desc = status_desc;
    }

	need = packer->len + 16 + strlen(desc);
    http_packer_resize(packer, need);
	sprintf(packer->buf + packer->len, "HTTP/1.1 %d %s\r\n", status, desc);
	packer->len = need - 1;

    return 0;
}

void http_packer_body(struct http_packer* packer, const char* body, size_t body_len) {
    size_t need;
	char len_buf[8];

	assert(body_len <= 65536);

	sprintf(len_buf, "%d", body_len);
	http_packer_header(packer, "Content-Length", len_buf);

    need = packer->len + 3 + body_len;
	http_packer_resize(packer, need);
	packer->buf[packer->len] = '\r';
	packer->buf[packer->len + 1] = '\n';

	if (body_len > 0) {
		memcpy(packer->buf + packer->len + 2, body, body_len);
	}
	
	//sprintf(packer->buf + packer->len, "\r\n%.*s", body_len, body);
	packer->len = need - 1;
	packer->buf[packer->len] = 0;
}

