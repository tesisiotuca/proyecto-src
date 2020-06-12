
#Pasos previos
#En este caso se ocupo el editor VIM
sudo apt-get install vim
sudo apt-get update

#Estableciendo parámetros de IP

#ruta del archivo de configuración
sudo vim /etc/network/interfaces

#si se creo el adaptador Host-Only Network
auto enp0s8
iface enp0s8 inet static
address 192.168.215.101
submask 255.255.255.0

#asignará la ip 192.168.215.101 al adaptador

#Reiniciando Servicios de Red de Adaptadores
sudo /etc/init.d/networking restart

