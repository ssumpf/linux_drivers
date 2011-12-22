#
# Add Atheros blob driver to objects to be linked.
#
INC_DIR     += $(REP_DIR)/src/madwifi/include

EXT_OBJECTS += i386-elf.hal.o

$(TARGET): i386-elf.hal.o

i386-elf.hal.o: $(REP_DIR)/src/madwifi/contrib/hal/public/i386-elf.hal.o.uu
	$(VERBOSE)uudecode -o $@ $<
