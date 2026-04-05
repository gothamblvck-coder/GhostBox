#!/usr/bin/env python3
import os
import sys
import subprocess
import shutil
import ctypes

# --- Kernel Enforcement Constants ---
PR_SET_NO_NEW_PRIVS = 38
PR_SET_PDEATHSIG = 1
SIGKILL = 9

def enable_kernel_lockdown():
    lockdown_file = "/sys/kernel/security/lockdown"
    if not os.path.exists(lockdown_file):
        print("[!] Warning: Kernel Lockdown not supported.")
        return

    with open(lockdown_file, "r") as f:
        current_state = f.read()

    if "[none]" in current_state:
        print("[*] Shielding Kernel: Enabling Integrity Lockdown...")
        try:
            subprocess.run(["sudo", "sh", "-c", f"echo integrity > {lockdown_file}"], check=True)
            print("[+] Kernel Locked.")
        except subprocess.CalledProcessError:
            print("[!] Failed to lock kernel. Security degraded.")
    else:
        print("[*] Kernel Security: Lockdown already active.")

def harden_process():
    libc = ctypes.CDLL('libc.so.6')
    # Force NO_NEW_PRIVS (The point of no return)
    libc.prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)
    libc.prctl(PR_SET_PDEATHSIG, SIGKILL)

def launch_ghost_box(target_args):
    enable_kernel_lockdown()

    original_bin = target_args[0]
    full_path = shutil.which(original_bin) or os.path.abspath(original_bin)
    
    if not os.path.exists(full_path):
        print(f"[!] Error: {original_bin} not found.")
        sys.exit(1)

    app_dir = os.path.dirname(full_path)
    bin_name = os.path.basename(full_path)
    
    # CRITICAL: Absolute path to the mandatory seccomp file
    script_dir = os.path.dirname(os.path.realpath(__file__))
    seccomp_path = os.path.join(script_dir, "seccomp.bpf")

    # --- ENFORCEMENT CHECK ---
    if not os.path.exists(seccomp_path):
        print("\n[FATAL ERROR] SECURITY VIOLATION: seccomp.bpf NOT FOUND!")
        print(f"Expected at: {seccomp_path}")
        print("The Ghost Box cannot deploy without the Sentinel filter. Deployment aborted.")
        sys.exit(1) # Kill the script here
    
    print("[+] Sentinel Verified: Seccomp filter located.")

    bwrap_cmd = [
        "bwrap",
        "--unshare-all",
        "--share-net",
        "--new-session",
        "--die-with-parent",
        "--tmpfs", "/",
        "--proc", "/proc",
        "--dev", "/dev",
        "--tmpfs", "/sys",
        "--ro-bind", "/usr", "/usr",
        "--ro-bind", "/bin", "/bin",
        "--ro-bind", "/lib", "/lib",
        "--ro-bind", "/lib64", "/lib64",
        "--ro-bind", "/etc/ssl", "/etc/ssl",
        "--ro-bind", "/etc/resolv.conf", "/etc/resolv.conf",
        "--hostname", "obsidian-node",
        "--unshare-user",
        "--uid", "1000",
        "--gid", "1000",
        "--tmpfs", "/home/ghost",
        "--dir", "/run/user/1000",
        "--setenv", "LIBGL_ALWAYS_SOFTWARE", "1",
        "--setenv", "GALLIUM_DRIVER", "llvmpipe",
        "--ro-bind", app_dir, "/app",
        "--clearenv",
        "--setenv", "HOME", "/home/ghost",
        "--setenv", "PATH", "/usr/bin:/bin:/app",
        "--setenv", "XDG_RUNTIME_DIR", "/run/user/1000",
        "--as-pid-1",
        "--seccomp", "3" # Pointing to the FD we pass below
    ]

    # Wayland setup
    xdg_runtime = os.environ.get("XDG_RUNTIME_DIR")
    wayland_display = os.environ.get("WAYLAND_DISPLAY")
    if xdg_runtime and wayland_display:
        wayland_path = os.path.join(xdg_runtime, wayland_display)
        bwrap_cmd += ["--bind", wayland_path, f"/run/user/1000/{wayland_display}"]
        bwrap_cmd += ["--setenv", "WAYLAND_DISPLAY", wayland_display]

    final_bin = os.path.join("/app", bin_name)
    full_command = bwrap_cmd + [final_bin] + target_args[1:]

    print(f"[*] DEPLOYING GHOSTBOX: {bin_name}")
    
    # Open the file and pass its File Descriptor (3) to the child process
    try:
        with open(seccomp_path, "rb") as seccomp_file:
            subprocess.run(
                full_command, 
                preexec_fn=harden_process, 
                pass_fds=[seccomp_file.fileno()] # Ensures it maps to FD 3
            )
    except Exception as e:
        print(f"[!] Critical Launch Failure: {e}")
    finally:
        print("[*] BOX DISSOLVED. AMNESIA COMPLETE.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 ghostbox.py <app_name_or_path>")
        sys.exit(1)
    launch_ghost_box(sys.argv[1:])
