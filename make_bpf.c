#include <seccomp.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // 1. Initialize: Allow most calls, but sniper-block the dangerous ones.
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

    // 2. THE "NO-ESCAPE" RULES: Kill the process if it tries these:
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(ptrace), 0);     // Stop spying/debugging
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mount), 0);      // Stop mounting disks
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(umount2), 0);    // Stop unmounting
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(kexec_load), 0); // Stop kernel hijacking
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(reboot), 0);     // Stop system restarts
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(swapon), 0);     // Stop memory swapping
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(init_module), 0);// Stop loading drivers

    // 3. Export to binary (The kernel requires a multiple of 8 bytes).
    int fd = open("seccomp.bpf", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to create seccomp.bpf");
        return 1;
    }
    seccomp_export_bpf(ctx, fd);
    
    close(fd);
    seccomp_release(ctx);
    printf("[+] seccomp.bpf generated successfully (The Sentinel is active).\n");
    return 0;
}
