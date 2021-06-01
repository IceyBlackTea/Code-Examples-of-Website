def parse_xdp_action(action):
    if action == 0:
        return "XDP_ABORTED"

    if action == 1:
        return "XDP_DROP"

    if action == 2:
        return "XDP_PASS"

    if action == 3:
        return "XDP_TX"

    if action == 4:
        return "XDP_REDIRECT"

    return "ERROR"