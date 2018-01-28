CFLAGS+=-Wall -Wextra -O2
SRCDIR=src
OBJDIR=bin

OBJS =		$(OBJDIR)/sh_remotedata.o \
		$(OBJDIR)/sh_string.o \
		$(OBJDIR)/sudohulk.o

sudohulk: $(OBJS)
	$(CC) $(CFLAGS) -o sudohulk $(OBJS)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean all

all: sudohulk

clean:
	rm -f $(OBJS) sudohulk
