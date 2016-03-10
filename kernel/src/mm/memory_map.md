## Memory Map for RazOS

| Begin       | End (incl.) | Purpose
| ----------- | ----------- | -------
| `0000_0000` | `0000_0fff` | Null page. Not mapped in.
| `0000_1000` | `000f_ffff` | Low memory (1 MiB).
| `0010_0000` | *???*       | The kernel is loaded here (at 1 MiB) by GRUB.
| `xxx0_0000` | `0fbf_ffff` | Kernel heap area (align to 4MiB).
| `0fc0_0000` | `0fff_fff0` | Kernel stack. 4MiB. Starts at 252 MiB.
| `1000_0000` | *???*       | User program image (.text, .bss etc)
| `xxx0_0000` | `ffbf_ffff` | User heap area (align to 4MiB).
| `ffc0_0000` | `ffff_fff0` | User stack (4 MiB).
