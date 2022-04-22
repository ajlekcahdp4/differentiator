# new Makefile
CC= gcc

CFLAGS ?= -Wall -Werror -Wextra -O3

COMMONINC = -I./include
CFLAGS += $(COMMONINC)


DEBUG = -g

CFLAGS += $(DEBUG)

SRCDIR = ./sources/
BUILDDIR =./build/

CSRCS = $(SRCDIR)main.c		   				 \
		$(SRCDIR)lexer/lexer.c  			 \
		$(SRCDIR)parser/parser.c			 \
		$(SRCDIR)derivatives/derivatives.c	 \
		$(SRCDIR)optimization/optimization.c \
		$(SRCDIR)writetex/writetex.c		 \
		$(SRCDIR)dump_tree/dump_tree.c		 \
		$(SRCDIR)tree/tree.c


SUBS := $(CSRCS)
SUBS := $(subst $(SRCDIR), $(BUILDDIR), $(SUBS))

OBJS  = $(SUBS:.c=.o)
DEPS = $(SUBS:.c=.d)

all: $(OBJS)
	$(CC) $(CFLAGS) $^ -o differentiate -lm

$(BUILDDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -c -o $@


$(BUILDDIR)%.d: $(SRCDIR)%.c
	@echo "Collecting deps for $< ..."
	@mkdir -p $(dir $@)
	@$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@


include $(DEPS)


.PHONY: clean
clean:
	rm -rf $(OBJS) $(DEPS) *.aux *.fdb_latexmk *.fls *.log *.pdf *.tex *synctex.gz differentiate *.dot *.png vgcore.*
