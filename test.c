//////////////////////////////////////////////////////////////////////////////////////
// Mail: radiotail86@gmail.com
// About the details of license, please read LICENSE
//////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "http_packer.h"

int main(int argc, char** argv) {
    struct http_packer packer;

	http_packer_init(&packer, HTTP_PACKER_REQUEST);
	http_packer_request_line(&packer, "GET", "www.baidu.com", "/", 80);
	http_packer_header(&packer, "User-Agent", "rick");
	http_packer_header(&packer, "Connection", "Keep-Alive");
	http_packer_body(&packer, "hello", strlen("hello"));
	printf("REQUEST: \n%s\nEND\n\n", packer.buf);

	http_packer_reset(&packer, HTTP_PACKER_RESPONSE);
    http_packer_respone_line(&packer, 200, "OK");
    http_packer_header(&packer, "xxx", "yyy");
	http_packer_body(&packer, "world", strlen("world"));
    printf("RESPONSE: \n%s\nEND\n", packer.buf);

	http_packer_destroy(&packer);

	getchar();
}



