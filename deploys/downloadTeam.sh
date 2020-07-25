cp runSharedLibrary.sh ~
cp runTeam.sh ~
cd ~
mkdir team
mv runSharedLibrary.sh team/
mv runTeam.sh team/
cd team/
git clone https://github.com/sisoputnfrba/tp-2020-1c-Qbits-Racers.git
cd tp-2020-1c-Qbits-Racers/
git checkout Team
git pull
cd ..
