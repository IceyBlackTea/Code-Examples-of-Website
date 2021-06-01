#!/usr/bin/python

"""
watch_kernel_func.py
Example: Watch the functions of the loadable kernel module
1. Compile the loadable kernel module
2. Insert the loadable kernel module
3. Create the character device in `/dev`
4. Do read and write operations with the device
5. Check the output of this script
"""

from bcc import BPF
from bcc.utils import printb


def watch_kernel_func():
    bpf = BPF(src_file="watch_kernel_func.c")
    bpf.attach_kprobe(event="device_read", fn_name="watch_device_read")
    bpf.attach_kprobe(event="device_write", fn_name="watch_device_write")

    def print_event(cpu, data, size):
        event = bpf["events"].event(data)

        printb(b"pid: %-8d type: %-8s buffer: %s" % (
            event.pid,
            'read' if event.type == 0 else 'write',
            event.buffer
        ))

    bpf["events"].open_perf_buffer(print_event)

    print("Start watching...")

    while True:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            break


def run():
    watch_kernel_func()


def main():
    run()


if __name__ == "__main__":
    main()
