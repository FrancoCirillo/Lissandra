unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd FileSystem
valgrind --tool=drd --check-stack-var=yes --exclusive-threshold=40000 --free-is-write=yes --log-file=drdFS.txt -v Default/FileSystem 