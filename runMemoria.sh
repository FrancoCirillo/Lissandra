unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd Memoria
read -p "Introduzca el IP de la Memoria (Vacio para 127.0.0.3): " IPMemoria
Default/Memoria "$IPMemoria"