#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <string.h>

#define bf_resize(buff, size) do { \
    void *newBuff = malloc(sizeof(buff.content[0]) * size); \
    if (buff.content == NULL) { \
        buff.content = newBuff; \
        buff.capacity = (size); \
        break; \
    } \
    memcpy(newBuff, buff.content, sizeof(buff.content[0]) * (buff).count); \
    free((buff).content); \
    buff.content = newBuff; \
    buff.capacity = (size); \
} while(false)

#define bf_append(buff, value) do { \
    if (buff.content == NULL) { \
        bf_resize(buff, 10); \
    } else if (buff.count + 1 >= buff.capacity) { \
        bf_resize(buff, buff.count * 2); \
    } \
    buff.content[buff.count++] = (value); \
} while(false)

#define bf_free(buff) do { \
    free(buff.content); \
    memset(&buff, 0, sizeof(buff)); \
} while(false);
#endif
