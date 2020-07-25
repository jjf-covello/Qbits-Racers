cp runSharedLibrary.sh ~
cp runBroker.sh ~
cd ~
mkdir broker
mv runSharedLibrary.sh broker/
mv runBroker.sh broker/
cd broker/
git clone https://github.com/sisoputnfrba/tp-2020-1c-Qbits-Racers.git
cd tp-2020-1c-Qbits-Racers/
git checkout Broker
git pull
cd ..