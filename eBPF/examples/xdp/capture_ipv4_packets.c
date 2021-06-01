/*
 * capture_ipv4_packets.c
 * Example: Capture ipv4 packets
 * Do filter actions on the ipv4 packet if it sent to the filtered port with tcp/udp.
 * Send custom packet information data to user space.
 */

#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

// Create a BPF table for pushing out custom event data
// to user space via a perf ring buffer.
BPF_PERF_OUTPUT(events);

// Create a struct for outputting
struct data_t {
    unsigned int ip_src_addr;
    unsigned int trans_proto;
    unsigned int src_port;
    unsigned int dst_port;
    unsigned int action;
};

static inline unsigned int parse_ipv4_trans_proto(void *data, u64 nh_off, void *data_end) {
    struct iphdr *iph = data + nh_off;

    if ((void*)&iph[1] > data_end)
        return 0;

    return iph->protocol;
}

static inline unsigned int parse_ipv4_src_addr(void *data, u64 nh_off, void *data_end) {
    struct iphdr *iph = data + nh_off;

    if ((void*)&iph[1] > data_end)
        return 0;

    return iph->saddr;
}

static inline unsigned int parse_tcp_src_port(void *data, u64 nh_off, void *data_end) {
    struct tcphdr *tcph = data + nh_off;

    if ((void*)&tcph[1] > data_end)
        return 0;

    return htons(tcph->source);
}

static inline unsigned int parse_tcp_dst_port(void *data, u64 nh_off, void *data_end) {
    struct tcphdr *tcph = data + nh_off;

    if ((void*)&tcph[1] > data_end)
        return 0;

    return htons(tcph->dest);
}

static inline unsigned int parse_udp_src_port(void *data, u64 nh_off, void *data_end) {
    struct udphdr *udph = data + nh_off;

    if ((void*)&udph[1] > data_end)
        return 0;

    return htons(udph->source);
}

static inline unsigned int parse_udp_dst_port(void *data, u64 nh_off, void *data_end) {
    struct udphdr *udph = data + nh_off;

    if ((void*)&udph[1] > data_end)
        return 0;

    return htons(udph->dest);
}

int xdp_filter(struct xdp_md *ctx) {
    // compiler options -Dmacro[=defn]
    int filter_port = FILTERPORT;
    int filter_action = FILTERACTION;

    void* data_end = (void*)(long)ctx->data_end;
    void* data = (void*)(long)ctx->data;
    struct ethhdr *eth = data;

    uint16_t h_proto;    // header protocol number
    uint64_t nh_off = 0; // next header offset

    // To get the next header, the offset is the size of ethernet frame header
    nh_off = sizeof(*eth);

    // If the packets is too short, return
    if (data + nh_off  > data_end)
        return XDP_PASS;

    // Get next header protocol number
    h_proto = eth->h_proto;

    // Parse ipv4
    if (h_proto == htons(ETH_P_IP)) {
        // Get the source ipv4 address and protocol number
        uint32_t ip_src_addr = parse_ipv4_src_addr(data, nh_off, data_end);
        uint32_t trans_proto = parse_ipv4_trans_proto(data, nh_off, data_end);

        // To get the next header, the offset is the size of ip packet header
        nh_off += sizeof(struct iphdr);

        uint32_t src_port = 0;
        uint32_t dst_port = 0;

        if (trans_proto == IPPROTO_TCP) {
            src_port = parse_tcp_src_port(data, nh_off, data_end);
            dst_port = parse_tcp_dst_port(data, nh_off, data_end);
        } else if (trans_proto == IPPROTO_UDP) {
            src_port = parse_udp_src_port(data, nh_off, data_end);
            dst_port = parse_udp_dst_port(data, nh_off, data_end);
        }

        int action = XDP_PASS;

        if (dst_port == filter_port) {
            action = filter_action;
        }

        // Use the struct data_t to output
        struct data_t data = {};
        data.ip_src_addr = ip_src_addr;
        data.trans_proto = trans_proto;
        data.src_port = src_port;
        data.dst_port = dst_port;
        data.action = action;

        events.perf_submit(ctx, &data, sizeof(data));

        return action;
    }

    // Operate packets
    // drop -> XDP_DROP
    // pass -> XDP_PASS
    return XDP_PASS;
}
