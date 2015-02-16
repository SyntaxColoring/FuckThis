/* Mnemonics for JVM opcodes. */

#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

#define OP_CALOAD        0x34
#define OP_CASTORE       0x55
#define OP_ICONST_N(n)   (0x03+(n))
#define OP_SIPUSH        0x11
#define OP_RETURN        0xb1
#define OP_NEWARRAY      0xbc
#define OP_ASTORE_N(n)   (0x4b+(n))
#define OP_IADD          0x60
#define OP_GETSTATIC     0xb2
#define OP_SWAP          0x5f
#define OP_DUP           0x59
#define OP_DUP2          0x5c
#define OP_INVOKEVIRTUAL 0xb6

#define ATYPE_CHAR  5

#endif
