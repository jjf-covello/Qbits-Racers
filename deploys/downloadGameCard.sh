cp runSharedLibrary.sh ~
cp runGameCard.sh ~
cd ~
mkdir gamecard
mv runSharedLibrary.sh gamecard/
mv runGameCard.sh gamecard/
cd gamecard/
git clone https://github.com/sisoputnfrba/tp-2020-1c-Qbits-Racers.git
cd tp-2020-1c-Qbits-Racers/
git checkout Game-Card
git pull
cd ..
