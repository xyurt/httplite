#include "httplite.h"
#include <stdio.h>

void main() {
    const char *request = "GET /index.html HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n"; /* A typical http request*/
    size_t len = 66; /* Length of request without the null terminator */

    http_message message; /* Http message structure */
    if (http_parse_message(request, len, &message)) {
        printf("Method: %.*s\n", (int)message.part1_length, message.part1);
        printf("Path: %.*s\n", (int)message.part2_length, message.part2);
        printf("Version: %.*s\n", (int)message.part3_length, message.part3);

        const char *name, *value;
        size_t name_len, value_len;

        printf("Headers:\n");
        while (http_next_header(&message, &name, &name_len, &value, &value_len)) {
            if (name == NULL || name_len == 0) break;
            printf("  %.*s: %.*s\n", (int)name_len, name, (int)value_len, value);
        }

        if (message.next_length != 0) {
            printf("Body:\n%.*s\n", (int)message.next_length, message.next);
        }
    }
    else {
        printf("Failed to parse HTTP message\n");
    }
}
