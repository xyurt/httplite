#ifndef HTTP_PARSER 
#define HTTP_PARSER 
typedef struct http_message {
    const char *part1; /* Method field for request, Version field for response */
    size_t part1_length;
    const char *part2; /* Path field for request, Code field for response */
    size_t part2_length;
    const char *part3; /* Version field for request, Reason field for response */
    size_t part3_length;
    const char *next;  /* Pointer to continue reading the headers and the body as a stream */
    size_t next_length;
} http_message;
static int http_next_header(http_message *message, const char **name, size_t *name_length, const char **value, size_t *value_length) {
    if (message->next_length < 2) return 0;
    if (message->next[0] == '\r' && message->next[1] == '\n') {
        message->next += 2;
        message->next_length -= 2;
        *name_length = *value_length = *name = *value = 0;
        return 1;
    }
    size_t next_offset = 0;
    *name_length = *value_length = 0;
    *name = &message->next[next_offset];
    int colon_found = 0;
    while (next_offset + 1 < message->next_length && !(colon_found = (message->next[next_offset++] == ':'))) (*name_length)++;
    if (!colon_found || *name_length == 0) return 0;
    *value = &message->next[next_offset];
    while (next_offset + 1 < message->next_length && message->next[next_offset++] != '\r') (*value_length)++;
    if (*value_length == 0 || next_offset + 1 >= message->next_length || message->next[next_offset++] != '\n') return 0;
    message->next = &message->next[next_offset];
    message->next_length -= next_offset;
    return 1;
}
static int http_parse_message(const char *buffer, size_t buffer_size, http_message *out) {
    size_t buffer_offset = 0;
    out->part1_length = out->part2_length = out->part3_length = out->next_length = 0;
    out->part1 = &buffer[buffer_offset];
    while (buffer_offset + 1 < buffer_size && buffer[buffer_offset++] != ' ') out->part1_length++;
    if (out->part1_length == 0 || buffer[buffer_offset] == ' ' || buffer[buffer_offset] == '\r' || buffer[buffer_offset] == '\n') return 0;
    out->part2 = &buffer[buffer_offset];
    while (buffer_offset + 1 < buffer_size && buffer[buffer_offset++] != ' ' && buffer[buffer_offset] != '\r') out->part2_length++;
    if (out->part2_length == 0 || buffer[buffer_offset] == ' ' || buffer[buffer_offset] == '\n') return 0;
    out->part3 = &buffer[buffer_offset];
    while (buffer_offset + 1 < buffer_size && buffer[buffer_offset++] != '\r') out->part3_length++;
    if (buffer_offset + 1 >= buffer_size || buffer[buffer_offset++] != '\n') return 0;
    out->next = &buffer[buffer_offset];
    out->next_length = buffer_size - buffer_offset;
    return 1;
}
#endif /* HTTP_PARSER */
