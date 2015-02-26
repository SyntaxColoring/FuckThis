Introduction
============
Fuck This compiles [Brainfuck](https://esolangs.org/wiki/Brainfuck) programs
to Java bytecode.  It emits standalone Java SE 5 class files.  Fuck This is
written in ANSI C (C90).

License
=======
This software is released under the terms of the [WTFPL](LICENSE.txt).  Naturally.

Usage Example
=============
    $ echo '++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..++
    +.>>.<-.<.+++.------.--------.>>+.>++.' > hello.bf
    $ ./ft hello.bf Hello
    $ file Hello.class
    Hello.class: compiled Java class data, version 49.0 (Java 1.5)
    $ java Hello
    Hello World!

For detailed instructions, supply the `--help` option.

Portability Notes
=================================================
Brainfuck, despite its simplicity, is not a well defined language.  Here is how
Fuck This behaves with respect to the language's ambiguities:

  - The array is 30,000 cells long and each cell is a byte.
  - Incrementing a value of 255 yields 0 and decrementing a value of 0 yields 255.
  - Moving the index past array boundaries is allowed, but trying to access data
    there will crash the program.
  - The `,` command returns newlines exactly as they appear on the host
    filesystem (a 13 followed by a 10 on Windows, and just a 10 elsewhere).
  - The `,` command returns -1 (255) when EOF is reached.

This is known as *Fuck This Flavored Brainfuck.*

Building
========
Just run `make` in the root Fuck This directory to compile the executable.
`make clean` is also supported.
