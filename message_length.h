#ifndef MESSAGE_LENGTH_H
#define MESSAGE_LENGTH_H

#define MESSAGE_LENGTH_TYPE unsigned short
#define MESSAGE_LENGTH_PREFIX_SIZE sizeof(MESSAGE_LENGTH_TYPE)
#define MESSAGE_MAX_SIZE 65536 + MESSAGE_LENGTH_PREFIX_SIZE

#include <cstdlib>
#include <cstring>

void* prependMessageLength(const void* message, MESSAGE_LENGTH_TYPE length);
#endif