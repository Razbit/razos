## Memory Map for RazOS

| Begin       | End (incl.) | Purpose
| ----------- | ----------- | -------
| `0000_0000` | `0000_0fff` | Null page. Not mapped in.
| `0000_1000` | `000f_ffff` | Low memory. Untouched.
| `0010_0000` | *???*       | The kernel is loaded here by GRUB.
| `xxx0_0000` | `0fbf_ffff` | Kernel heap area (align to 4MiB).
| `0fc0_0000` | `0fff_fff0` | Syscall/kernel stack.
| `1000_0000` | *???*       | User program image (.text, .bss etc)
| `xxx0_0000` | `ffbf_ffff` | User heap area (align to 4MiB).
| `ffc0_0000` | `ffff_fff0` | User stack.
