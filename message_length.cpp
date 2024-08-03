#include "message_length.h"

void* prependMessageLength(const void* message, MESSAGE_LENGTH_TYPE length) {
    char* result = (char*) malloc(MESSAGE_LENGTH_PREFIX_SIZE + length);
    memcpy(result, &length, MESSAGE_LENGTH_PREFIX_SIZE);
    memcpy(result + MESSAGE_LENGTH_PREFIX_SIZE, message, length);
    return result;
}