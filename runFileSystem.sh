unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd FileSystem
read -p "Introduzca el IP del FS (Vacio para 127.0.0.2) : " IPFS
Default/FileSystem "$IPFS"