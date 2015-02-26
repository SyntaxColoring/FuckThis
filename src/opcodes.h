/* opcodes.h - Mnemonics for JVM opcodes.  See chapter 6 of the Java Virtual
   Machine Specification, Second Edition:
   https://docs.oracle.com/javase/specs/jvms/se5.0/html/Instructions.doc.html */

#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum opcode
{
	OP_ICONST_0      = 0x03,
	OP_SIPUSH        = 0x11,
	OP_BALOAD        = 0x33,
	OP_BASTORE       = 0x54,
	OP_DUP2          = 0x5c,
	OP_SWAP          = 0x5f,
	OP_IADD          = 0x60,
	OP_IFEQ          = 0x99,
	OP_GOTO          = 0xa7,
	OP_RETURN        = 0xb1,
	OP_GETSTATIC     = 0xb2,
	OP_INVOKEVIRTUAL = 0xb6,
	OP_NEWARRAY      = 0xbc
};

#define OP_ICONST_N(n) (OP_ICONST_0+(n))
#define OP_ICONST_N_MIN -1
#define OP_ICONST_N_MAX 5
		
enum array_type /* Used with OP_NEWARRAY. */
{
	ARRAY_TYPE_BYTE = 8
};

#endif
