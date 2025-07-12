# httplite

A minimal HTTP/1.1 request line and header parser written in pure C.

## Overview

httplite is a lightweight, zero-allocation HTTP/1.1 parser designed for blazing-fast parsing using only pointer arithmetic. It parses HTTP request lines and headers directly from the input buffer without copying data.

## Usage Example

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

## How It Works

- Parses HTTP/1.x request line (method, path, version).
- Iterates over headers with zero-copy pointer arithmetic.
- No dependencies, dynamic memory allocation or even the standart library.

## API

- int http_parse_message(const char *buffer, size_t buffer_size, http_message *out);
  Parses the start line and initializes the message for header parsing.

- int http_next_header(http_message *message, const char **name, size_t *name_length, const char **value, size_t *value_length);
  Iterates headers one by one; returns 1 while headers remain, 0 at end or error.

## Performance Notes
- Zero-copy parsing ensures minimal CPU and memory overhead.
- Suitable for high-throughput or embedded environments.

## Limitations

- Supports HTTP/1.x request line and headers only.
- Expects CRLF (\r\n) line endings.
- Does not validate message bodies or headers, just does its primary job which is parsing.
- No support for multiline headers or chunked encoding.

## License

MIT License

---

Minimal, blazing-fast HTTP/1.1 parsing using only pointer arithmetic for zero-copy efficiency.
