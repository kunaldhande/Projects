mode=" "

if [ "$1" = "s" ]; then
    mode="s"
elif [ "$1" = "m" ]; then
    mode="m"
fi

#ssh-agent bash

sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install oracle-java8-installer

echo "Java Vesion: "
java -version

sudo apt-get install build-essential

ssh-keygen -t rsa -P ""
chmod 400 *.pem
ssh-add ~/*.pem

cat ~/hadoop-bashrc >> ~/.bashrc

if [ $mode = "s" ]; then
    ./hadoop-single.sh
elif [ $mode = "m" ]; then
    sudo chmod 777 /etc/hosts
    cat hadoop-multi-hosts >> /etc/hosts
    ./hadoop-multi.sh
fi
