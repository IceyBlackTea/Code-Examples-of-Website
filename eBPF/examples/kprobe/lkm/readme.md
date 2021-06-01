# Kprobe example - tracing LKM

Kprobe, get data from kernel functions

## How to run
1. Use `make` to compile the loadable kernel module
2. Use `make install` to insert the lkm
3. Use `dmesg | tail -1` to check the major number
4. Use `mknod /dev/lks_example c [major num] 0` 
   to create the character device
5. Run `watch_kernel_func.py` with root privileges
6. Do read and write operations with the device
7. Check the output of `watch_kernel_func.py`

## Example

### Compile lkm

It looks like:
```
# make
make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
make[1]: Entering directory '/usr/src/linux-headers-$(shell uname -r)'
  CC [M]  $(PWD)/hello.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      $(PWD)/lkm_driver.mod.o
  LD [M]  $(PWD)lkm_driver.ko
make[1]: Leaving directory '/usr/src/linux-headers-$(shell uname -r)'
```

Use `ls` and check the *.ko file in the current directory.

### Test lkm and get the major number

Use `make test` to check if the lkm is ok after compiling.

You can also get the major number from the output by this way.

It looks like:

```
......
[ 7565.954219] lkm_example module loaded with device major number 510
[ 7565.959764] Goodbye, World!
```

### install/uninstall lkm

Use `make install` / `make uninstall`.

It will not show any other messages in the shell if succeeded.

### do read and write operations

Use `cat /dev/lks_example` to read the device.

Create a file and use `cat [file] > /dev/lks_example`to write the device.

It will not show any other messages in the shell if succeeded.

### the output of `watch_kernel_func.py`

It looks like:

```
Start watching...
pid: 14839    type: read     buffer: 
pid: 14839    type: read     buffer: Hello, World!

pid: 14874    type: write    buffer: This is a test device file.

pid: 14890    type: read     buffer: 
pid: 14890    type: read     buffer: This is a test device file.
   
```

## Problems

### use "asm/uaccess.h"

If you use the header file "asm/uaccess.h",
the compilation always goes failed and it shows like:

```
./arch/x86/include/asm/uaccess.h: In function 'set_fs':
./arch/x86/include/asm/uaccess.h:32:9: error: dereferencing pointer to incomplete type 'struct task_struct'
  current->thread.addr_limit = fs;
```

```
error: implicit declaration of function 'copy_to_user' [-Werror=implicit-function-declaration]
     if (copy_to_user(user_buffer, msg_ptr + *offset, len))
```

#### Solution

It's better to use the header file "linux/uaccess.h" rather than "asm/uaccess.h".
