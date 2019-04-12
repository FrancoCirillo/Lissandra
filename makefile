.PHONY: default all clean
all: default

default: HerramientasMake KernelMake MemoriaMake FileSystemMake

HerramientasMake:
	cd Herramientas/Debug; make

KernelMake:
	cd Kernel/Default; make

MemoriaMake:
	cd Memoria/Default; make

FileSystemMake:
	cd FileSystem/Default; make

clean:
	cd Herramientas/Debug; make clean
	cd Kernel/Default; make clean
	cd Memoria/Default; make clean
	cd FileSystem/Default; make clean