sudo apt install -y libasio-dev #libboost-all-dev libssl-dev libcurl4-openssl-dev libjsoncpp-dev cmake
sudo apt install -y libgtest-dev
mkdir build
cd build
CC=/usr/bin/gcc CXX=/usr/bin/g++ cmake ..
make -j`nproc` all
./LinuxServer
./LinuxClient
