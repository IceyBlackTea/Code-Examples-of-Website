#!/usr/bin/python

"""
capture_ipv4_packets.py
Example: Capture ipv4 packets
1. Set the device name, filter port and filter action
2. Run this python script
3. Send ipv4 packets from other PCs
4. Check the output of this script
"""

from bcc import BPF
from bcc.utils import printb

from utils import dec2ip, parse_ip_protocol, parse_xdp_action


def filter_ipv4_packets(device, filter_port, filter_action):
    bpf = BPF(
        src_file="capture_ipv4_packets.c",
        cflags=[
            "-w",
            "-DFILTERPORT=%d" % filter_port,
            "-DFILTERACTION=%s" % filter_action,
        ],
        device=None
    )

    fn = bpf.load_func("xdp_filter", BPF.XDP, None)

    bpf.attach_xdp(device, fn, flags=0)

    def print_event(cpu, data, size):
        event = bpf["events"].event(data)

        printb(b"%-16s  %-8s  %-8s  %-8s  %-12s" % (
            dec2ip(event.ip_src_addr),
            parse_ip_protocol(event.trans_proto),
            str(event.src_port) if event.src_port != 0 else '-',
            str(event.dst_port) if event.dst_port != 0 else '-',
            parse_xdp_action(event.action)
        ))

    bpf["events"].open_perf_buffer(print_event)

    print("ip                protocol  src_port  dst_port  action")

    while True:
        try:
            bpf.perf_buffer_poll()
        except KeyboardInterrupt:
            break

    bpf.remove_xdp(device, 0)


def run():
    device = "enp4s0f0"
    filter_port = 80
    filter_action = "XDP_DROP"

    filter_ipv4_packets(device, filter_port, filter_action)


def main():
    run()


if __name__ == "__main__":
    main()
