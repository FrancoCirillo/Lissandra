unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd KERNEL_MOCK
read -p "Introduzca los numeros de las Memorias: " numerosMemoria
Debug/KERNEL_MOCK $numerosMemoria