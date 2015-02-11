/* Mnemonics for JVM opcodes. */

#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

#define OP_CALOAD      0x34
#define OP_CASTORE     0x55
#define OP_ICONST_N(n) (0x03+(n))
#define OP_SIPUSH      0x11
#define OP_RETURN      0xb1
#define OP_NEWARRAY    0xbc
#define OP_ASTORE_N(n) (0x4b+(n))
#define OP_IADD        0x60

#define ATYPE_CHAR  5

#endif
