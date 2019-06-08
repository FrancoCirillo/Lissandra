echo "mkdir git y clone"
cd ..
echo "Descargando commons"
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make install
cd ..
mkdir /home/utnso/lissandra-checkpoint
echo "Configuracion inicializada"
cd /home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua
make
echo "Lissandra compilado!"
echo "Ejecucion lista! 20-04"