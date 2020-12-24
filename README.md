# linux-kernel-shadow-ssh
## TL;DR
Hiding SSH public keys in SSH server using a kernel agent.

Depends on kallsyms which means it will work on most of the Linux distros except embedded systems which probably compiled without it.

Tested on Linux kernel version: 4.19.91.

## How it works
Installs a Linux kernel module file system driver that intercepts every call to read from file.

The file system filter is based on this project: https://github.com/Rhydon1337/linux-kernel-filesystem-filter

The kernel module has a list of hidden keys that can be changed dynamically.

Interception method flow:
* Check if the path that the read call should read from contains ssh/authorized_keys
  * If it does, we check if the caller process is ssh
    * If it is ssh, we return the full content of the file
    * If it isn't ssh, we hide all the keys and return the content of the file without the hidden keys
  * If it does't, we just return the regular content of the wanted file

### Problems & Solutions
We hook the read_iter file system function and the file system is using the linux kernel **cache**.

#### First problem
When I overrite the page that returns from the real file system read_iter it changes the cache directly and in the next time I will call the real file system read_iter it will return my previously written data and not what written on the disk.
Solution: before calling the real file system read_iter we will drop all the inode cached pages using truncate_inode_page.

#### Second problem
The second problem is caused from the first solution, what happend if we write the ssh/authorized_keys to disk and immediately do all of our stuff.
It will cause all of data to disapper because of the fact that the file system won't write it immediately to the disk it will cache it first and then in our call to truncate_inode_page we drop the data that was written and when we will call the real file system read_iter there is no data that will be returned.
Solution: flush the inode to the disk using fliemap_flush and then truncate_inode_page.

## Limitations
Currently, there is no support in case that someone does cat -n 3 to the file it will cause kernel panic.

## Usage
cd linux-kernel-shadow-ssh

make

insmod shadow_ssh.ko

{ssh-copy-id} // copy your ssh public key to the authorized keys

cat hidden_key > /dev/shadow_ssh

Now, you can ssh to the machine and no one will be able to see that you added a key.

DONE!!!
