# httplite_extended.h — Extension to httplite.h
This extended version builds on top of the original httplite.h, adding stricter validation and useful features without changing the base functionality.

## Usage:
Place both httplite.h and httplite_extended.h in the same directory.
Include httplite_extended.h in your source file.
Use the extended functions provided.

## What it adds:
Validates HTTP method names (e.g., GET, POST, etc.)
Validates HTTP version (e.g., HTTP/1.1)
Validates response status code range
Trims whitespace from header values
Allows strict matching against a list of known headers

## Example:
```C
#include "httplite_extended.h"
#include <stdio.h>

void main() {
    const char *response = "HTTP/1.1 504\r\nHost: 127.0.0.1\r\nContent-Type: text/plain\r\nAllow:\r\nExpect:\r\n\r\nResponse body <3"; /* A typical http request*/
    size_t len = 92; /* Length of request without the null terminator */

    http_response res;
    if (http_parse_response_ext(response, len, &res)) {
        printf("Version: %.*s\n", res.version_length, res.version);
        printf("Code: %.*s\n", res.code_length, res.code);
        printf("Reason: %.*s\n", res.reason_length, res.reason);

        http_header hdr;
        while (http_next_header_ext(&res, &hdr)) {
            if (hdr.name_length == 0) break;
            printf("  %.*s: %.*s\n", hdr.name_length, hdr.name, hdr.value_length, hdr.value);
        }

        if (res.next_length != 0) {
            printf("Body:\n%.*s\n", res.next_length, res.next);
        }
    }
    else {
        printf("Failed to parse HTTP message\n");
    }
}
```

#### Im open to suggestions so let me know if you have anything to suggest or fix or even point out ;)
