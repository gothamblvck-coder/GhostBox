#include <seccomp.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 1. Initialize: Allow all by default, then pick-pocket the dangerous ones
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

    // 2. The "No-Escape" Rules
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(ptrace), 0);     // Stops spying on other apps
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mount), 0);      // Stops accessing drives
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(umount2), 0);    // Stops unmounting drives
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(kexec_load), 0); // Stops kernel hijacking
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(reboot), 0);     // Stops system restarts
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(swapon), 0);     // Stops memory sniffing
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(swapoff), 0);

    // 3. Export to the "Multiple of 8" binary format
    int fd = open("seccomp.bpf", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create seccomp.bpf");
        return 1;
    }
    seccomp_export_bpf(ctx, fd);
    
    close(fd);
    seccomp_release(ctx);
    printf("[+] seccomp.bpf generated successfully (Multiple of 8 bytes).\n");
    return 0;
}
