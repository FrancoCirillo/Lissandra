# **Como PCs en el agua.**
## _Lisandra - TP 1er cuatrimestre 2019_

### Integrantes: 
* Cirillo Franco
* Diaz Mariano
* Diaz Rodrigo
* Maidana Daiana
* Luquet Azul

------------
#### Como usar:

- Clonar-  en `/home/utnso/git/`
- Hacer `cd tp-2019-1c-Como-PCs-en-el-agua`
- Ejecutar el makefile con `make` (Recomendacion:  hacer `make clean` antes)
- Para correr cada proceso, hacerlo desde _runProceso_.sh. Si dice que no tiene permisos, ejecutar `chmod 777 runProceso.sh` para cada .sh ¯\\\_(ツ)_/¯

Si prefieren correrlo desde la consola, hay que ejecutar los procesos desde su carpeta (donde están los .config), **no** desde Default/.Por ejemplo:
```bash
cd tp-2019-1c-Como-PCs-en-el-agua/FileSystem
Default/FileSystem
```
Esto es porque la ubicación que les dimos a los .config
También, si se ejecuta desde la consola, hay que tener en cuenta que tenemos nuestra Shared Library ubicada en Herramientas/

------------

#### Uso de las Shared Libraries:

Antes de abrir algun proceso, hay que avisarle al SO adonde están *nuestra* Shared Library. Esto se hace corriendo el siguiende comando (cada vez que se ejecute):
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/Debug/
```
Luego sin cerrar la terminal ejecutar el programa.

**Esto se hace automaticamente si se ejecuta desde los .sh**

##### Consideraciones si se quiere correr desde Eclipse:
Para usar nuestra Shared Library desde Eclipse:
1. En Run Configurations, seleccionamos la que queremos modificar (Kernel, Memoria o FileSystem)
2. En la Pestaña Environment clickeamos "Select..."
3. Elegimos LD_LIBRARY_PATH y le damos Ok
4. La seleccionamos y la editamos con "Edit..."
5. Le **agregamos** al final de lo que ya está en el campo "Value" el path a nuestra biblioteca, en este caso le agregamos `:/home/utnso/git/tp-2019-1c-Como-PCs-en-el-agua/Herramientas/Debug/` (incluidos los ":")
6. "Apply"
Hacer esto para Kernel, Memoria y FileSystem
