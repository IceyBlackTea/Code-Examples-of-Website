def dec2ip(dec):
    ip_str = str(bin(dec))[2:]

    while len(ip_str) < 32:
        ip_str = "0" + ip_str

    ip_decs = []

    for i in range(4):
        bin_str = ip_str[i * 8: i * 8 + 8]
        ip_decs.insert(0, str(int(bin_str, 2)))

    return '.'.join(ip_decs)