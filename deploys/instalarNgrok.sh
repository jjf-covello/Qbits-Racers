cd ~
wget https://faq.utnso.com.ar/ngrok.zip
unzip ngrok.zip

if [ $1 -eq 1 ]
then
	./ngrok authtoken 1f0bPRJW3w0ZtIRa51pLyW2u62h_2eDaK1MKfwftmgNBYd7Rs
elif [ $1 -eq 2 ]
then
	./ngrok authtoken 1bune6CHjW2mh7vUsKnGrPSBfsq_6N91Whr7yHyi9Fum5cRty
elif [ $1 -eq 3 ]
then
	./ngrok authtoken 1cAvzMWcFsOggdtj1mcQQ6cqC15_28m5ypccPx8RAtQyEcAnN
elif [ $1 -eq 4 ]
then
	./ngrok authtoken 1cMjFyWSVbNrxk0VMTJcMKfwOiW_3sT8mNLRcwH9bxDcPyspK
elif [ $1 -eq 5 ]
then
	./ngrok authtoken 1f0ba9sSXNG6PgJMYL9rfOYhZ0X_3JHqfVPJyipf6iDvXJ9PZ
fi

echo "Ahora corre el comando: ./ngrok tcp PUERTO"

