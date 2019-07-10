unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd Memoria
rm -f memoria.config
cp 1memoria.config memoria.config
valgrind --show-leak-kinds=all --leak-check=full --track-origins=yes -v --log-file="memValTest.txt" Default/Memoria