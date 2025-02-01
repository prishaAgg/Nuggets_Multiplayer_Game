# Top-Level Makefile

# Phony targets
.PHONY: all clean server vision grid libcs50 support structures

# Default target
all: libcs50 support structures grid vision server

# Build the server (depends on grid, vision, and structures)
server: grid vision structures libcs50 support
	@echo "Building server..."
	$(MAKE) -C server

# Build the grid module (depends on structures and libcs50)
grid: structures libcs50 support
	@echo "Building grid module..."
	$(MAKE) -C grid

# Build the vision module (depends on grid, structures, and libcs50)
vision: grid structures libcs50 support
	@echo "Building vision module..."
	$(MAKE) -C vision

# Build the structures module (depends on libcs50 and support)
structures: libcs50 support
	@echo "Building structures module..."
	$(MAKE) -C structures

# Build the libcs50 library
libcs50:
	@echo "Building libcs50 library..."
	$(MAKE) -C libcs50

# Build the support library (creates support.a)
support:
	@echo "Building support library..."
	$(MAKE) -C support support.a

# Clean all modules and libraries
clean:
	@echo "Cleaning all subdirectories..."
	$(MAKE) -C server clean
	$(MAKE) -C vision clean
	$(MAKE) -C grid clean
	$(MAKE) -C libcs50 clean
	$(MAKE) -C support clean
	$(MAKE) -C structures clean