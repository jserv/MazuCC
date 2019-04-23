# MazuCC

MazuCC is a minimalist C compiler with x86_64 code generation.
It is intended to support partial C99 language features while keeping the code
as small and simple as possible.


## Build

Run make to build:
```shell
$ make
```

MazuCC comes with unit tests. To run the tests, give "check" as an argument:
```shell
$ make check
```

MazuCC is known to work on both GNU/Linux and macOS.

Use MazuCC to compile C source:
```shell
$ ./mzcc < sample/nqueen.c
```

You will get the generated x86_64 assembly in AT&T syntax. The output can be
assembled and linked into a valid executable:
```shell
$ ./mzcc < sample/nqueen.c > sample/nqueen.s
$ gcc -no-pie -o sample/nqueen sample/nqueen.s
```

If MazuCC is compiled and executed on macOS, the above argument `-no-pie`
should be eliminated.

Reference output of MazuCC-compiled `sample/nqueen`:
```
Q . . . . . . .
. . . . Q . . .
. . . . . . . Q
. . . . . Q . .
. . Q . . . . .
. . . . . . Q .
. Q . . . . . .
. . . Q . . . .
```

Alternatively, you can dump internal abstract syntax tree:
```shell
echo 'struct {int x; char y; struct { int t; } z; } a;' | ./mzcc --dump-ast
```

The expected output in S-expression form:
```
(decl (struct (int)
              (char)
              ((struct (int)))) a)
```

## Acknowledge

MazuCC is heavily inspired by [8cc](https://github.com/rui314/8cc).

## License

MazuCC is freely redistributable under the BSD 2 clause license. Use of
this source code is governed by a BSD-style license that can be found in the
LICENSE file.
