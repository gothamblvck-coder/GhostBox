📦 GhostBox
The Hardened, Zero-Persistence Sandbox for Kali & Parrot OS

GhostBox is a specialized Python-based security wrapper designed to isolate untrusted tools and binaries within Kali Linux and Parrot OS. Built on the industrial-strength Bubblewrap (bwrap) core, it ensures that what happens in the box, stays in the box.
🚀 Key Features

    Volatile RAM-Home: Mounts a tmpfs over your home directory. All data created during the session evaporates the moment you close the tool.

    Zero-Knowledge Isolation: Your real files, SSH keys, and browser data are physically invisible to the sandboxed process.

    Kernel Hardening: Uses --cap-drop ALL to strip every kernel capability. Even if a tool gains "root" inside the box, it is powerless against your host.

    Identity Masking: Automatically spoofs the hostname to ghost-box and unshares PID namespaces, preventing the tool from seeing other running programs.

⚖️ Why GhostBox?

GhostBox was built for security researchers who find Firejail too bloated or risky due to its SUID-root architecture.
Feature	GhostBox	Firejail / Others
Attack Surface	Minimal: Uses unprivileged namespaces.	Large: Uses SUID-root binaries.
Persistence	None: Everything runs in RAM.	Optional: Often writes to disk by default.
Speed	Native: Zero-overhead kernel isolation.	Native: Similar performance.
Simplicity	High: One script, no complex profiles.	Low: Thousands of complex config files.
🛠 Pros & Cons
✅ Pros

    Near-Native Performance: No VM overhead.

    Read-Only Core: System directories (/usr, /bin, /etc) are locked as Read-Only.

    Network Ready: Supports tools like nmap while maintaining total filesystem isolation.

    Automated Cleanup: "Die-with-parent" logic prevents zombie processes.

❌ Cons

    Stateless: You must manually move files out if you want to save them.

    CLI Focused: No GUI configurator; designed for the terminal.

    OS Specific: Optimized specifically for Debian-based security distros.

📥 Installation
Bash

# 1. Install dependencies
sudo apt update && sudo apt install bwrap -y

# 2. Clone and make executable
chmod +x ghostbox

# 3. (Optional) Move to your path
sudo mv ghostbox /usr/local/bin/

📖 Usage

Simply prefix any command with ghostbox:
Bash

# Safely run a suspicious python script
ghostbox python3 exploit.py

# Open a browser with an invisible home directory
ghostbox firefox-esr

# Run network tools with zero host exposure
ghostbox nmap -sV <target>GhostBox: The Hardened Sandbox for Kali Linux & Parrot OS

GhostBox is a specialized, Python-powered isolation wrapper designed specifically for the security-centric ecosystems of Kali Linux and Parrot OS. While most sandboxes are built for general desktop privacy, GhostBox is engineered for the "Live Environment" mindset: it provides a high-security, zero-persistence container for running untrusted tools, scripts, and binaries without risking your host’s integrity.

Built upon the industrial-grade Bubblewrap (bwrap) core, GhostBox creates an invisible wall between your sensitive OS and the tools you execute.
Why GhostBox is Better Than Firejail

While Firejail is a popular choice, it has historically suffered from a large attack surface due to its complex SUID-root architecture. GhostBox takes a more modern, "less-is-more" approach:

    Minimized Attack Surface: GhostBox utilizes unprivileged user namespaces via bwrap. It doesn’t rely on a massive SUID-root binary, significantly reducing the risk of a sandbox escape.

    Volatile Filesystem: Unlike Firejail, which often maps your real home directory, GhostBox mounts a tmpfs (RAM-disk) over /home. The application sees an empty, fresh environment. The moment the process ends, every file created or modified vanishes from existence.

    Aggressive Capability Stripping: GhostBox doesn't just "limit" permissions; it uses --cap-drop ALL to strip the process of every kernel capability. Even if a process gains "root" inside the box, it remains powerless against your hardware and kernel.

    No Profile Bloat: Firejail relies on thousands of specific application profiles that can become outdated. GhostBox provides a standardized, hardened "Security First" environment that works universally for CLI tools.

Pros & Cons
Pros	Cons
Instant Ephemerality: All session data is stored in RAM and purged on exit.	No GUI Configurator: Managed entirely via terminal (ideal for Kali/Parrot users).
Identity Masking: Spoofer logic hides your real hostname and PID list.	Read-Only Core: Prevents tools from updating system-wide components.
Near-Native Speed: Leverages kernel namespaces rather than heavy VM emulation.	CLI Knowledge: Requires a basic understanding of how to pass arguments via terminal.
Leak-Proof: Prevents processes from "seeing" your real home files or SSH keys.	Stateless: You must manually move files out if you wish to keep them.
Technical Guardrails

GhostBox enforces a strict security protocol for every execution:

    Filesystem Isolation: Core system paths (/usr, /bin, /lib, /etc) are mounted as Read-Only.

    PID Unsharing: The sandboxed tool cannot see or interact with any other programs running on your system.

    Network Control: Provides a shared network stack for tools like nmap or msfconsole, but keeps the underlying identity (hostname) masked as ghost-box.

    Death-Parent Logic: If the terminal or parent process is killed, the sandbox and all its children are instantly terminated—no "zombie" processes left behind.

Quick Start
Bash

# Analyze a suspicious binary
ghostbox ./unknown_tool

# Run a browser without exposing your local files
ghostbox firefox-esr --private-window

# Perform a scan with zero local traces
ghostbox nmap -Pn <target_ip>
