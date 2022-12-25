CC = clang
BIN = dude.exe
PATHSEP = \\
BUILDDIR = build
SRC = main.c lexer/lexer.c parser/parser.c
OBJ = $(subst /,\, $(SRC:%.c=$(BUILDDIR)/%.o))
CFLAGS = -Wall -g

RM = del 			# rm -rf 
RMDIR = rd /s /q 	# rm -rf 

test:
	echo $(OBJ)

all: clean $(BIN)
 
clean:
	$(RMDIR) $(BUILDDIR)
	$(RM) $(BIN) $(OBJ)	
	mkdir $(BUILDDIR)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)
	
$(BUILDDIR)$(PATHSEP)%.o: %.c
	mkdir $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
