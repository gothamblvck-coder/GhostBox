#include <seccomp.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	// Start with a permissive filter for stability
	scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
	
	// THE IRON CURTAIN: Kill process on escape attempts
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(ptrace), 0);     // No Spying
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mount), 0);      // No Escaping Namespace
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(umount2), 0);    // No altering mounts
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(kexec_load), 0); // No Kernel hijacking
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(reboot), 0);     // No system restarts
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(init_module), 0);// No loading drivers
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(finit_module), 0);
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(delete_module), 0);
	
	// Export to binary
	int fd = open("seccomp.bpf", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
		perror("Failed to create seccomp.bpf");
		return 1;
	}
	seccomp_export_bpf(ctx, fd);
	close(fd);
	seccomp_release(ctx);
	printf("[+] seccomp.bpf generated (The Sentinel is active).\n");
	return 0;
}
