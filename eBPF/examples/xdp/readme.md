# XDP example

XDP, capture ipv4 packets

## How to run
1. Set the device name, filter port and filter action in `capture_ipv4_packets.py`
2. Run `capture_ipv4_packets.py` with root privileges
3. Send ipv4 packets from other PCs
4. Check the output of `capture_ipv4_packets.py`

## Example
It looks like:

```
ip                protocol  src_port  dst_port  action
192.168.31.10     UDP       118       80        XDP_DROP
192.168.31.10     UDP       111       90        XDP_PASS
192.168.31.10     TCP       220       80        XDP_DROP
192.168.31.10     TCP       124       90        XDP_PASS
192.168.31.10     ICMP      -         -         XDP_PASS
```

## Problems

It seems that there is no problem yet.

## Others

+ Some struct or others you may use:
  [reference](../../docs/reference.md)

+ `uc` usage: [command](../../docs/command.md#nc)