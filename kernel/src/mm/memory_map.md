## Memory Map for RazOS

| Begin       | End (incl.) | Purpose
| ----------- | ----------- | -------
| `0000_0000` | `0000_0fff` | Null page. Not mapped in.
| `0000_1000` | `000f_ffff` | Low memory. Untouched.
| `0010_0000` | *???*       | The kernel is loaded here by GRUB.
| *???*       | `0fbf_ffff` | Kernel dynamic allocation area.
| `0fc0_0000` | `0fff_ffff` | Kernel stack.
| `1000_0000` | *???*       | User program image (.text, .bss etc)
| *???*       | `ff7f_ffff` | User dynamic allocation area.
| `ff80_0000` | `ffbf_ffff` | User stack.
| `ffc0_0000` | `ffff_efff` | Recursively mapped page tables
| `ffff_f000` | `ffff_ffff` | Recursively mapped page directory
