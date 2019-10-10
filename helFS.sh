unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"../Herramientas/Debug/"
cd ..
cd ..
rm -rf lfs-base
cd /home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua
cd FileSystem
valgrind --tool=helgrind --log-file=helgrindFS.txt -v Default/FileSystem 