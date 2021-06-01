# USDT example

USDT, get data from tracepoints

## How to run
1. Compile the app program, use `make`
2. Run `catch_usdt.py` with root privileges
3. Run the app
4. Check the output of `catch_usdt.py`

## Example
It looks like:

```
pid       argc      argv[0]   argv[1]
26659     1         -         -
26661     2         hello     -       
26663     3         hello     world 
```

## Problems

### Can't find "sys/sdt.h"

When you try to compiler the app program, it may shows like:

```
# gcc -o hello_usdt main.cc
main.cc:1:21: fatal error: sys/sdt.h: No such file or directory
compilation terminated.
```

#### Solution

**Install systemtap-sdt-dev.**

```
# for Ubuntu
sudo apt update
sudo apt install systemtap-sdt-dev
```

### Can't find "string.h"

When you try to operate strings in ebpf programs, it may shows like:

```
/virtual/main.c:16:10: fatal error: 'string.h' file not found
#include <string.h>
         ^~~~~~~~~~
1 error generated.
```

#### Solution

**BPF programs cannot use functions from the libc.**

Try to modify the code to functions supported by ebpf.
