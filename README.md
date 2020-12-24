# linux-kernel-shadow-ssh
## TL;DR
Hiding SSH public keys in SSH server using a kernel agent.

Depends on kallsyms which means it will work on most of the Linux distros except embedded systems which probably compiled without it.

Tested on Linux kernel version: 4.19.91.

## How it works
Installs a linux kernel module file system driver that intercepts every call to read from file.

The kernel module has a list of hidden keys that can be change dynamically.

Intercption method flow:
* Check if the path that the read call should read from conatins ssh/authorized_keys
  * If it does, we check if the caller process is ssh
    * If it is ssh, we return the full content of the file
