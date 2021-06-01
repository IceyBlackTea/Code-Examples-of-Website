/*
 * watch_kernel_func.c
 * Example: Watch the functions of the loadable kernel module
 * Get the buffer string from the parameter of the kernel functions.
 * Send custom packet information data to user space.
 */

#include <uapi/linux/ptrace.h>

// Create a BPF table for pushing out custom event data
// to user space via a perf ring buffer.
BPF_PERF_OUTPUT(events);

#define READ 0
#define WRITE 1

struct data_t {
    u32 pid;
    int type;
    char buffer[128];
};

int watch_device_read(struct pt_regs *ctx, struct file *flip, char *buffer, size_t len) {
    struct data_t data;

    data.pid = bpf_get_current_pid_tgid();

    data.type = READ;

    /*
     * bpf_probe_read_kernel_str: This copies a NULL terminated string from kernel address space
     *                            to the BPF stack, so that BPF can later operate on it.
     * int bpf_probe_read_kernel_str(void *dst, int size, const void *src)
     */
    bpf_probe_read_kernel_str(data.buffer, sizeof(data.buffer), buffer);

    events.perf_submit(ctx, &data, sizeof(data));

    return 0;
}

int watch_device_write(struct pt_regs *ctx, struct file *flip, char *buffer, size_t len) {
    struct data_t data;

    data.pid = bpf_get_current_pid_tgid();

    data.type = WRITE;

    /*
     * bpf_probe_read_kernel_str: This copies a NULL terminated string from kernel address space
     *                            to the BPF stack, so that BPF can later operate on it.
     * int bpf_probe_read_kernel_str(void *dst, int size, const void *src)
     */
    bpf_probe_read_kernel_str(data.buffer, sizeof(data.buffer), buffer);

    events.perf_submit(ctx, &data, sizeof(data));

    return 0;
}