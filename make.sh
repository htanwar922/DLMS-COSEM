sudo apt install -y libasio-dev #libboost-all-dev libssl-dev libcurl4-openssl-dev libjsoncpp-dev cmake
sudo apt install -y libgtest-dev
mkdir build
cd build
CC=/usr/bin/arm-linux-gnueabihf-gcc CXX=/usr/bin/arm-linux-gnueabihf-g++ cmake ..
make -j`nproc` all
cd src/Linux
./LinuxServer
./LinuxClient
