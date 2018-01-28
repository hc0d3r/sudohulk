# Sudohulk

Change sudo command

## supported architectures:

- [x] x86_64
- [ ] x86
- [ ] arm

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

## Demo
[![asciicast](https://asciinema.org/a/159627.png)](https://asciinema.org/a/159627)
