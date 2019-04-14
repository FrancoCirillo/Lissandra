.PHONY: default all clean
all: default

default: HerramientasMake KernelMake MemoriaMake FileSystemMake

HerramientasMake:
	cd Herramientas/Debug; make all

KernelMake:
	cd Kernel/Default; make all

MemoriaMake:
	cd Memoria/Default; make all

FileSystemMake:
	cd FileSystem/Default; make all

clean:
	cd Herramientas/Debug; make clean
	cd Kernel/Default; make clean
	cd Memoria/Default; make clean
	cd FileSystem/Default; make clean