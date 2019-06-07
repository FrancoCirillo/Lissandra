echo "mkdir git y clone"
cd ..
echo "Descargando commons"
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make install
cd ..
mkdir /home/utnso/lissandra-checkpoint
echo "Configuracion inicializada"