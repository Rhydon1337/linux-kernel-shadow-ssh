#pragma once

#include <linux/fs.h>

void initialize_filter(char* filesystem_file_operation_name);

ssize_t file_read_iter(struct kiocb *iocb, struct iov_iter *to);