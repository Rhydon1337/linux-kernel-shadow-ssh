#pragma once
#include <linux/fs.h>

int device_open(struct inode *inode, struct file *file);

int device_close(struct inode *inode, struct file *file);

ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

ssize_t device_write(struct file *, const char *, size_t, loff_t *);