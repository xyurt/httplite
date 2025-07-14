#include "httplite.h"
#include <ctype.h>
#include <string.h>
typedef struct http_string_entry {
    const char *name;
    int length;
} http_string_entry;
typedef struct http_header {
    char *name;
    int name_length;
    char *value;
    int value_length;
} http_header;
const http_string_entry http_headers[] = {
{"Cache-Control",13},{"Connection",10},{"Date",4},{"Pragma",6},{"Trailer",7},
{"Transfer-Encoding",17},{"Upgrade",7},{"Via",3},{"Warning",7},{"Accept",6},
{"Accept-Charset",14},{"Accept-Encoding",15},{"Accept-Language",15},{"Authorization",13},
{"Expect",6},{"From",4},{"Host",4},{"If-Match",8},{"If-Modified-Since",17},
{"If-None-Match",13},{"If-Range",8},{"If-Unmodified-Since",19},{"Max-Forwards",11},
{"Proxy-Authorization",19},{"Range",5},{"Referer",7},{"TE",2},{"User-Agent",10},
{"Accept-Ranges",13},{"Age",3},{"ETag",4},{"Location",8},{"Proxy-Authenticate",18},
{"Retry-After",11},{"Server",6},{"Vary",4},{"WWW-Authenticate",16},{"Allow",5},
{"Content-Encoding",16},{"Content-Language",16},{"Content-Length",14},
{"Content-Location",16},{"Content-MD5",11},{"Content-Range",13},{"Content-Type",12},
{"Expires",7},{"Last-Modified",13},{NULL,0}
};
const http_string_entry http_methods[] = {{"OPTIONS",7},{"GET",3},{"HEAD",4},{"POST",4},{"PUT",3},{"DELETE",6},{"TRACE",5},{"CONNECT",7},{0,0}};
const http_string_entry http_versions[] = {{"HTTP/1.1",8},{0,0}};
const int http_code_range[4] = { 100, 3, 599, 3 }; /* [100, 599], the values after the min and max are the length of the int strings */
typedef struct http_response {
    const char *version;
    int version_length;
    const char *code;
    int code_length;
    const char *reason;
    int reason_length;
    const char *next;
    int next_length;
} http_response;
typedef struct http_request {
    const char *method;
    int method_length;
    const char *path;
    int path_length;
    const char *version;
    int version_length;
    const char *next;
    int next_length;
} http_request;
static int string_to_integer(const char *str, int str_len) {
    int result = 0;
    int i;
    for (i = 0; i < str_len; i++) {
        if (str[i] < '0' || str[i] > '9') return -1;
        result = result * 10 + (str[i] - '0');
    }
    return result;
}
static int trim_string_buffer(char *buffer, int buffer_size, char **out_buffer) {
    int buffer_offset = 0;
    while (buffer_offset < buffer_size && isspace((unsigned char)buffer[buffer_offset])) buffer_offset++;
    if (buffer_offset >= buffer_size) return 0;
    while (buffer_size > buffer_offset && isspace((unsigned char)buffer[buffer_size - 1])) buffer_size--;
    if (buffer_size <= buffer_offset) return 0;
    *out_buffer = &buffer[buffer_offset];
    return buffer_size - buffer_offset;
}
static char is_string_in_kvp_list(const http_string_entry *list, const char *buffer, int buffer_size) {
    int i;
    for (i = 0; list[i].name != NULL; i++) {
        if (buffer_size != list[i].length) continue;
        if (memcmp(list[i].name, buffer, buffer_size) == 0) return 1;
    }
    return 0;
}
static char http_parse_request_ext(char *buffer, int buffer_size, http_request *out) {
    http_request msg;
    if (http_parse_message(buffer, buffer_size, (http_message *)&msg) == 0) return 0;
    if (msg.version_length == 0 || msg.version == 0) return 0;
    if (msg.method_length == 0 || msg.method == 0) return 0;
    if (msg.path_length == 0 || msg.path == 0) return 0;
    if (is_string_in_kvp_list(http_versions, msg.version, msg.version_length) == 0) return 0;
    if (is_string_in_kvp_list(http_methods, msg.method, msg.method_length) == 0) return 0;
    *out = msg;
    return 1;
}
static char http_parse_response_ext(char *buffer, int buffer_size, http_response *out) {
    http_response msg;
    if (http_parse_message(buffer, buffer_size, (http_message *)&msg) == 0) return 0;
    if (msg.version_length == 0 || msg.version == 0 || msg.code == 0 || msg.code_length == 0 || msg.code_length < http_code_range[1] || msg.code_length > http_code_range[3]) return 0;
    int code = string_to_integer(msg.code, msg.code_length);
    if (code == -1 || code < http_code_range[0] || code > http_code_range[2]) return 0;
    if (is_string_in_kvp_list(http_versions, msg.version, msg.version_length) == 0) return 0;
    *out = msg;
    return 1;
}
static char http_next_header_ext(http_message *message, http_header *out) {
    char *name, *value;
    int name_length, value_length;

    if (http_next_header(message, &name, &name_length, &value, &value_length) == 0) return 0;
    if (name_length == 0) {
        out->name = out->value = 0, out->name_length = out->value_length = 0;
        return 1;
    }
    out->name_length = name_length;
    out->name = name;
    if (is_string_in_kvp_list(http_headers, out->name, out->name_length) == 0) return 0;
    if (value_length != 0) {
        out->value_length = trim_string_buffer(value, value_length, &out->value);
        if (out->value_length == 0 || out->value == 0) {
            out->value_length = value_length;
            out->value = value;
        }
    }
    else {
        out->value_length = value_length;
        out->value = value;
    }
    return 1;
}