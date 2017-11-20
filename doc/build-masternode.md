MASTERNODE BUILD NOTES 
======================
Some notes on how to build a Straks Masternode in Ubuntu server. Using Windows Qt Client to configure.


System requirements
--------------------
An Ubuntu 16.04 64bit server is recommended with at least 768MB 
of memory and 15GB space available when running a Masternode.


Notes
-----
You need exactly 50000 STAK to run a Masternode. Masternode input must have at least 15 confirmations.
Building a new wallet.dat is also recommended (a seperate .conf is needed). Masternode earnings are
going to this wallet.dat and address. Send only newly earned coins away and pay attention for transaction fees.
To fix a broken accountaddress 0 (possible when sending huge amounts of coins away), do a self TX of 50000 STAK.


Start
-----
Open your Windows Straks-Qt Client and open the debug console.

    ```
	masternode genkey
	copy the generated code to a text file
	---
	getaccountaddress 0
	copy the generated address to a text file
	---
	encryptwallet "strong password"
    ---
	Send 50000 STAK to "address 0"
    ```
	

Build Instructions: Ubuntu & Debian
-----------------------------------

	$ adduser <NEW USER>
	$ passwd <PSW>
	$ gpasswd -a <NEW USER> sudo
	---
	$ sudo apt-get install build-essential
	$ sudo apt-get install libtool autotools-dev autoconf automake libssl-dev libevent-dev
	$ sudo apt-get install libboost1.54-all-dev
	$ sudo add-apt-repository ppa:bitcoin/bitcoin
	$ sudo apt-get install libdb4.8-dev libdb4.8++-dev
	$ sudo apt-get install miniupnpc*-dev
	$ sudo apt-get install git ntp make g++ gcc autoconf cpp ngrep iftop sysstat unzip
	$ sudo update-rc.d ntp enable
	$ sudo apt-get update
	$ sudo apt-get upgrade
	---
	optiopnal linux gui: $ sudo apt-get install lubuntu-desktop
	---
	optional, if problems with boost version: 
	$ sudo apt-get remove libboost*
	$ sudo apt-get purge libboost*
	$ sudo apt-get install libboost1.54-all-dev


	Swapfile:
	---------
	$ sudo dd if=/dev/zero of=/swapfile bs=2M count=1024
	$ sudo mkswap /swapfile
	$ sudo swapon /swapfile

	
	FireWall:
	---------
	$ sudo apt-get install ufw
	$ sudo ufw allow ssh/tcp
	$ sudo ufw limit ssh/tcp
	$ sudo ufw allow 7575/tcp
	$ sudo ufw logging on
	$ sudo ufw enable
	$ sudo ufw status


	Install STAK
	------------
	mkdir .straks
	cd .straks
	wget https://github.com/straks/straks/releases/download/straks-1.14.5/straks-1.14.5.tar.gz
	tar xvzf Linux.tar
	
	$ sudo cp straksd /usr/bin
	$ sudo chmod 775 /usr/bin/straksd

	
	Create a straks.conf in nano
	-------------------------------
	cd .straks
	nano

	---
	rpcuser=<anything>
	rpcpassword=<anything>
	rpcallowip=127.0.0.1
	maxconnections=100
	listen=1
	server=1
	daemon=1
    promode=1
	masternode=1
	masternodeprivkey=XXXXXX
	externalip=xxx.xxx.xxx.xxx:7575
	---

	save nano: Ctrl +  O
	exit nano: Ctrl +  X
	cd


	Create CRON
	-----------
	cd /etc/cron.d
	crontab -e
	2 (for nano)
	
	at bottom of newly created file, insert:
	---
	@reboot /usr/bin/straksd -shrinkdebugfile    [to start masternode  (or wherever you keep your daemon)]
	*/20 * * * * /usr/bin/straksd
	---
	save nano: Ctrl +  O
	exit nano: Ctrl +  X
	cd
	
	
	Manually start straksd
	-------------------------
	cd .straks
	./straksd	
	
	
	limcoinxd commands
	------------------
	./straksd getinfo
	./straksd masternode start 
	./straksd masternode stop
    ./straksd masternode current
	./straksd help
	
	
	Start Mining
	----------------------
	./straks.cli generate 100
	./straks.cli gethashespersec
	

Windows Straks-Qt Client configuration 
-----------------------------------------
(if using a seperate wallet.dat, a seperate straks.conf is needed)

	---
	go to straks.conf in %appdata%
	
	---
	rpcuser=<anything>
	rpcpassword=<anything>
	rpcallowip=127.0.0.1
	listen=1
	maxconnections=100
	masternode=1
    promode=1
	masternodeprivkey=XXXXXX
	externalip=xxx.xxx.xxx.xxx:7575
	---

	
	Windows Straks-Qt Client console commands
	--------------------------------------------
	masternode start 
	masternode stop
    masternode current
	getinfo
	help
