unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd Memoria
read -p "Introduzca en numero de Memoria: " numeroMemoria
Default/Memoria "$numeroMemoria"