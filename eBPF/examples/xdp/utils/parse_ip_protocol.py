def parse_ip_protocol(protocol):
    if protocol == 1:
        return "ICMP"

    if protocol == 6:
        return "TCP"

    if protocol == 17:
        return "UDP"

    return "OTHERS"
