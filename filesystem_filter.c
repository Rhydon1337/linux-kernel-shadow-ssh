#include "filesystem_filter.h"

#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/uio.h>
#include <linux/slab.h>

struct file_operations g_original_fops;

void initialize_filter(char* filesystem_file_operation_name) {
    unsigned long prev_cr0 = read_cr0();
    struct file_operations* original_filesystem_fops = (struct file_operations*)kallsyms_lookup_name(filesystem_file_operation_name);
    if (NULL == original_filesystem_fops) {
        printk("Unable to find the original filsystem fops\n");
        return;
    }
    g_original_fops = *original_filesystem_fops;

    write_cr0(prev_cr0 & (~ 0x10000));

    __sync_lock_test_and_set((unsigned long*)&original_filesystem_fops->read_iter, (unsigned long)file_read_iter);
    write_cr0(prev_cr0);
    __sync_synchronize();
}

ssize_t file_read_iter(struct kiocb *iocb, struct iov_iter *to) {
    ssize_t size = g_original_fops.read_iter(iocb, to);
    char* path = NULL;
    char* buffer = kmalloc(PATH_MAX, GFP_KERNEL);
    if (NULL != buffer) {
        path = dentry_path_raw(iocb->ki_filp->f_path.dentry, buffer, PATH_MAX);
        if (!IS_ERR(path)) {
            if (NULL != strstr(path, "ssh/authorized_keys")) {
                char* buffer1 = kmalloc(to->iov->iov_len, GFP_KERNEL);
                copy_from_user(buffer1, to->iov->iov_base, to->iov->iov_len);
                printk(KERN_INFO "Process: %s, read file: %s\n", current->comm, path);
            }
        }
        kfree(buffer);
    }
    return size;
}