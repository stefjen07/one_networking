#ifndef MESSAGE_H
#define MESSAGE_H
#include "message_length.h"

struct Message {
    char* content;
    MESSAGE_LENGTH_TYPE length;
};
#endif