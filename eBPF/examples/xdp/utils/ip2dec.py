def ip2dec(ip):
    ip_decs = ip.split(".")

    ip_bins = []

    for num in ip_decs:
        bin_str = bin(int(num))[2:]

        while len(bin_str) < 8:
            bin_str = "0" + bin_str

        ip_bins.insert(0, bin_str)

    return int(''.join(ip_bins))