unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd Kernel
read -p "Introduzca el IP del Kernel (Vacio para 127.0.0.4) : " IPKernel
Default/Kernel "$IPKernel"