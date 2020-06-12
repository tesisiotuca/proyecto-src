#Instalando ThingSpeak en Ubuntu 16.04.3 LTS server 

#Previos:
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
sudo sync
sudo reboot


sudo apt-get -y install build-essential git mysql-server mysql-client libmysqlclient-dev libxml2-dev libxslt-dev libssl-dev libsqlite3-dev
  
#Ingresar password para root en MySQL.

#Creando la base para Thingspeak     
mysql -u root -p   

CREATE USER 'thing'@'localhost' IDENTIFIED BY 'speak';
GRANT ALL PRIVILEGES ON thingspeak_test.* TO 'thing'@'localhost';
GRANT ALL PRIVILEGES ON thingspeak_development.* TO 'thing'@'localhost';
FLUSH PRIVILEGES;
exit

#Instalando Ruby
wget http://cache.ruby-lang.org/pub/ruby/2.1/ruby-2.1.5.tar.gz
tar xvzf ruby-2.1.5.tar.gz
cd ruby-2.1.5/

./configure
make
sudo make install
cd


#Instalando Thingspeak
git clone https://github.com/iobridge/thingspeak.git
cp thingspeak/config/database.yml.example thingspeak/config/database.yml
sudo apt-get install ruby-bundler ruby-dev
cd thingspeak/
sudo nano Gemfile
#añada '~> 0.3.18' después de mysql12 ---->  'mysql12', '~> 0.3.18'

bundle update eventmachine json mysql2 rake
bundle install
sudo nano config/initializers/abstract_mysql2_adapter.rb

  #Añada los siguientes 4 parámetros al archivo:
 
# config/initializers/abstract_mysql2_adapter.rb
class ActiveRecord::ConnectionAdapters::Mysql2Adapter
  NATIVE_DATABASE_TYPES[:primary_key] = "int(11) auto_increment PRIMARY KEY"
end 


sudo nano config/environment.rb
#Añadir la siguiente línea al final del archivo  
require File.expand_path('../../config/initializers/abstract_mysql2_adapter', __FILE__)


rake db:create
rake db:schema:load
rails server webrick

#Luego ingresa IP en el puerto :3000 -- 192.168.215.101:3000