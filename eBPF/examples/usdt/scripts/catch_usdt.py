#!/usr/bin/python

"""
scripts/catch_usdt.py
Example: USDT, get data from tracepoints
1. Compile the app program, use `make`
2. Run this python script with root privileges
3. Run the app
4. Check the output of this script
"""

from bcc import BPF, USDT
from bcc.utils import printb


def catch_usdt():
    usdt = USDT(path="../app/print")
    usdt.enable_probe(probe="probe-main", fn_name="trace_usdt")

    bpf = BPF(src_file='catch_usdt.c', usdt_contexts=[usdt])

    def print_event(cpu, data, size):
        event = bpf["events"].event(data)

        printb(b"%-8d  %-8d  %-8s  %-8s" % (
            event.pid,
            event.argc,
            event.argv_1 if event.argv_1 != '' else '-',
            event.argv_2 if event.argv_2 != '' else '-',
        ))

    bpf["events"].open_perf_buffer(print_event)

    print("pid       argc      argv[1]   argv[2]")

    while True:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            break


def run():
    catch_usdt()


def main():
    run()


if __name__ == "__main__":
    main()
