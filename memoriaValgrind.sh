unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd Memoria
rm -f memoria.config
cp 1memoria.config memoria.config
rm -f memValTestAnterior.txt
cp memValTest.txt memValTestAnterior.txt
rm -f memValTest.txt
valgrind --show-leak-kinds=all --leak-check=full -v --log-file="memValTest.txt" Default/Memoria