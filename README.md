## HTTP PACKER

一个简单的http协议格式拼装代码，可以用它拼装简单的http请求和应答消息。

// 初始化packer对象，设置packer类型
@ packer packer对象指针
@ type packer类型(HTTP_PACKER_REQUEST: 请求, HTTP_PACKER_RESPONSE: 应答)
int http_packer_init(struct http_packer* packer, enum http_packer_type type);

// 重置packer对象，设置packer类型。利用同一个packer对象重复拼装协议时必须先调用此函数重置packer对象
@ packer packer对象指针
@ type packer类型(HTTP_PACKER_REQUEST: 请求, HTTP_PACKER_RESPONSE: 应答)
int http_packer_reset(struct http_packer* packer, enum http_packer_type type);

// 销毁packer对象
@ packer packer对象指针
int http_packer_destroy(struct http_packer* packer);

// 拼装协议头
@ packer packer对象指针
@ field 协议头的域
@ value 协议头的值
int http_packer_header(struct http_packer* packer, const char* field, const char* value);

// 拼装请求行(只限HTTP_PACKER_REQUEST类型packer可用)
@ packer packer对象指针
@ method http方法(GET, POST ...)
@ host 域名
@ url 访问位置
@ port 端口
int http_packer_request_line(struct http_packer* packer, const char* method, const char* host, const char* url, int port);

// 拼装响应行(只限HTTP_PACKER_RESPONSE类型packer可用)
@ packer packer对象指针
@ status 状态码
@ desc 原因描述(基本状态描述都能自动生成, 无需使用者填写, 如果查不到会将desc的内容写入协议头)
int http_packer_respone_line(struct http_packer* packer, int status, const char* desc);

// 拼装内容
@ packer packer对象指针
@ body 内容
@ body_len 内容长度
void http_packer_body(struct http_packer* packer, const char* body, size_t body_len);

## EXAMPLE
http_packer_reset(&packer, HTTP_PACKER_RESPONSE);
http_packer_respone_line(&packer, 200, "OK");
http_packer_header(&packer, "xxx", "yyy");
http_packer_body(&packer, "world", strlen("world"));

