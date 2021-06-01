/*
 * scripts/catch-usdt.py
 * Example: USDT, get data from tracepoints
 * Get the pid of the traced program,
 * and read the two arguments set with the tracepoint.
 * Send custom information data to user space.
 */

#include <uapi/linux/ptrace.h>
#include <string.h>

// Create a BPF table for pushing out custom event data
// to user space via a perf ring buffer.
BPF_PERF_OUTPUT(events);

// Create a struct for outputting
struct data_t {
    u32 pid;
    int argc;
    char argv_1[64];
    char argv_2[64];
};

int trace_usdt(struct pt_regs *ctx) {
    struct data_t data;

    // Get the process id of the traced program
    data.pid = bpf_get_current_pid_tgid();

    uint64_t addr;
    uint64_t offset = 0;

    /*
     * bpf_usdt_readarg: read the argument from dtrace
     * int bpf_usdt_readarg(int argc, struct pt_regs *ctx, void *arg)
     *
     * bpf_usdt_readarg_p: read the value of the address
     *                     which the argument as a pointer pointed to
     * int bpf_usdt_readarg_p(int argc, struct pt_regs *ctx, void *buf, u64 len)
     */

    // Get the address of the first parameter passed from the tracepoint
    bpf_usdt_readarg(1, ctx, &addr);

    /*
     * bpf_probe_read_user: safely read size bytes from user address space
     *                      to the BPF stack
     * int bpf_probe_read_user(void *dst, int size, const void *src)
     *
     * bpf_probe_read_user_str: This copies a NULL terminated string from
     *                          user address space to the BPF stack
     * int bpf_probe_read_user_str(void *dst, int size, const void *src)
     */

    // Get the value of the struct passed from the tracepoint
    bpf_probe_read_user(&data.argc, sizeof(data.argc), (void *)addr);

    offset += sizeof(int);
    bpf_probe_read_user_str(data.argv_1, sizeof(data.argv_1), (void *) addr + offset);

    offset += sizeof(char) * 64;
    bpf_probe_read_user_str(data.argv_2, sizeof(data.argv_2), (void *) addr + offset);

    // Sent data to python scripts
    events.perf_submit(ctx, &data, sizeof(data));

    return 0;
}