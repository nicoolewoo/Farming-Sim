# These are variables for the GBA toolchain build

PROGNAME = App

OFILES = gba.o font.o main.o images/garbage.o images/basket.o images/title.o images/single.o images/fire.o images/family.o

# These are various settings used to make the GBA toolchain work

.PHONY: all
all: CFLAGS += $(CRELEASE) -I../shared
all: LDFLAGS += $(LDRELEASE)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include /opt/cs2110-tools/GBAVariables.mak

LDFLAGS += --specs=nosys.specs

# Adjust default compiler warnings and errors
CFLAGS += -Wstrict-prototypes -Wold-style-definition -Werror=vla

.PHONY: debug
debug: CFLAGS += $(CDEBUG) -I../shared
debug: LDFLAGS += $(LDDEBUG)
debug: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba: $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf: crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OFILES) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

.PHONY: release
release: CFLAGS += $(CRELEASE) -I../shared
release: LDFLAGS += $(LDRELEASE)
release: $(PROGNAME).gba
	@echo "Built $(PROGNAME).gba with release flags"

.PHONY: mgba
mgba: debug
	@echo "[EXECUTE] Running emulator mGBA"
	@echo "          Please see emulator.log if this fails"
	@mgba-qt $(PROGNAME).gba

.PHONY: gdb
gdb: debug
	@echo "[EXECUTE] Running Emulator MGBA with GDB"
	@echo "          Please see emulator.log if this fails"
	@mgba-qt $(PROGNAME).gba --gdb &
	# To give mgba some time to initialize and start up the GDB server.
	@sleep 0.5
	@gdb-multiarch -ex 'file $(PROGNAME).elf' -ex 'target remote 127.0.0.1:2345'

.PHONY: submit
submit: clean
	@rm -f submission.tar.gz
	@tar czvf submission.tar.gz *

.PHONY: clean
clean:
	@echo "[CLEAN] Removing all compiled files"
	rm -f *.o *.elf *.gba *.log */*.o *.sav
