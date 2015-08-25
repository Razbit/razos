/* This file is a part of the RazOS project
 *
 * kb.c -- Keyboard driver
 * 
 * Razbit 2014 
 */

#ifndef KB_H
#define KB_H

/* Key press event data format:
 * 0..7:  ASCII code
 * 8..15: Key code
 * 16..30: Flags
 */

/* KB event flags */
#define SCROLL_LOCK 0x0001
#define NUM_LOCK	0x0002
#define CAPS_LOCK	0x0004
#define SHIFT		0x0008
#define ALT			0x0010
#define CONTROL		0x0020
#define LSHIFT		0x0040
#define RSHIFT		0x0080
#define LALT 		0x0100
#define RALT 		0x0200
#define LCONTROL 	0x0400
#define RCONTROL 	0x0800
#define RELEASE 	0x1000
#define NUMPAD		0x2000
#define PAUSE       0x4000

/* Key codes */
#define KEY_ESC    0x01
#define KEY_1      0x02
#define KEY_2      0x03
#define KEY_3      0x04
#define KEY_4      0x05
#define KEY_5      0x06
#define KEY_6      0x07
#define KEY_7      0x08
#define KEY_8      0x09
#define KEY_9      0x0A
#define KEY_0      0x0B
#define KEY_MINUS  0x0C
#define KEY_EQUALS 0x0D
#define KEY_BSPC   0x0E
#define KEY_TAB    0x0F
#define KEY_Q      0x10
#define KEY_W      0x11
#define KEY_E      0x12
#define KEY_R      0x13
#define KEY_T      0x14
#define KEY_Y      0x15
#define KEY_U      0x16
#define KEY_I      0x17
#define KEY_O      0x18
#define KEY_P      0x19
#define KEY_RBRACK 0x1A
#define KEY_LBRACK 0x1B
#define KEY_RET    0x1C
#define KEY_LCTRL  0x1D
#define KEY_A      0x1E
#define KEY_S      0x1F
#define KEY_D      0x20
#define KEY_F      0x21
#define KEY_G      0x22
#define KEY_H      0x23
#define KEY_J      0x24
#define KEY_K      0x25
#define KEY_L      0x26
#define KEY_SCOL   0x27
#define KEY_SQUOT  0x28
#define KEY_BTICK  0x29
#define KEY_LSHIFT 0x2A
#define KEY_BSLASH 0x2B
#define KEY_Z      0x2C
#define KEY_X      0x2D
#define KEY_C      0x2E
#define KEY_V      0x2F
#define KEY_B      0x30
#define KEY_N      0x31
#define KEY_M      0x32
#define KEY_COMMA  0x33
#define KEY_DOT    0x34
#define KEY_SLASH  0x35
#define KEY_RSHIFT 0x36
#define NUM_ASTER  0x37
#define KEY_LALT   0x38
#define KEY_SPACE  0x39
#define KEY_CAPSL  0x3A
#define KEY_F1     0x3B
#define KEY_F2     0x3C
#define KEY_F3     0x3D
#define KEY_F4     0x3E
#define KEY_F5     0x3F
#define KEY_F6     0x40
#define KEY_F7     0x41
#define KEY_F8     0x42
#define KEY_F9     0x43
#define KEY_F10    0x44
#define KEY_NUML   0x45
#define KEY_SCRL   0x46
#define NUM_7      0x47
#define NUM_8      0x48
#define NUM_9      0x49
#define NUM_MINUS  0x4A
#define NUM_4      0x4B
#define NUM_5      0x4C
#define NUM_6      0x4D
#define NUM_PLUS   0x4E
#define NUM_1      0x4F
#define NUM_2      0x50
#define NUM_3      0x51
#define NUM_0      0x52
#define NUM_DOT    0x53
#define KEY_F11    0x57
#define KEY_F12    0x58
#define NUM_RET    0x59
#define KEY_RCTRL  0x5A
#define NUM_SLASH  0x5B
#define KEY_RALT   0x5C
#define KEY_HOME   0x5D
#define KEY_UP     0x5E
#define KEY_PGUP   0x5F
#define KEY_LEFT   0x60
#define KEY_RIGHT  0x61
#define KEY_END    0x62
#define KEY_DOWN   0x63
#define KEY_PGDOWN 0x64
#define KEY_INS    0x65
#define KEY_DEL    0x66
#define KEY_LGUI   0x67 /* Not yet */
#define KEY_RGUI   0x68 /* Not yet */
#define KEY_APPS   0x69 /* Not yet */
#define KEY_PRSCR  0x6A
#define KEY_PAUSE  0x6B

#endif /* KB_H */
