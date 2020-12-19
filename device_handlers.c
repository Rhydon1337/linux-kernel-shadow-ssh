#include "device_handlers.h"

#include <linux/uaccess.h>
#include <linux/slab.h>

#include "consts.h"

int device_open(struct inode *inode, struct file *file) {
    return SUCCESS;
}

int device_close(struct inode *inode, struct file *file) {
    return SUCCESS;
}

ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
}

ssize_t device_write(struct file * filep, const char * buffer, size_t len, loff_t * offset) {
}