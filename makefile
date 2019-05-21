.PHONY: default all clean
all: default
debug: HerramientasMake MemoriaMake FileSystemMockMake KernelMockMake

default: HerramientasMake KernelMake MemoriaMake FileSystemMake

HerramientasMake:
	cd Herramientas/Debug; make all

KernelMake:
	cd Kernel/Default; make all

MemoriaMake:
	cd Memoria/Default; make all

FileSystemMake:
	cd FileSystem/Default; make all

FileSystemMockMake:
	cd FS_MOCK/Debug; make all

KernelMockMake:
	cd KERNEL_MOCK/Debug; make all

clean:
	cd Herramientas/Debug; make clean
	cd Kernel/Default; make clean
	cd Memoria/Default; make clean
	cd FileSystem/Default; make clean