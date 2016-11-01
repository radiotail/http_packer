//////////////////////////////////////////////////////////////////////////////////////
// Mail: radiotail86@gmail.com
// About the details of license, please read LICENSE
//////////////////////////////////////////////////////////////////////////////////////

#ifndef http_packer_h
#define http_packer_h

#ifdef __cplusplus
extern "C" {
#endif

enum http_packer_type {
    HTTP_PACKER_REQUEST,
    HTTP_PACKER_RESPONSE,
};

#define HEADER_FIX_BUF 256
struct http_packer {
	size_t len;
    size_t size;
    char* buf;
    enum http_packer_type type;
    char fix_buf[HEADER_FIX_BUF];
};

int http_packer_init(struct http_packer* packer, enum http_packer_type type);
int http_packer_reset(struct http_packer* packer, enum http_packer_type type);
int http_packer_destroy(struct http_packer* packer);
int http_packer_header(struct http_packer* packer, const char* field, const char* value);
int http_packer_request_line(struct http_packer* packer, const char* method, const char* host, const char* url, int port);
int http_packer_respone_line(struct http_packer* packer, int status, const char* desc);
void http_packer_body(struct http_packer* packer, const char* body, size_t body_len);

#ifdef __cplusplus
}
#endif

#endif // http_packer_h

