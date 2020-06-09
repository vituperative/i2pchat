# I2P Chat AKA I2P Qt Messenger

## Screenshots

![screenshot-roster](https://user-images.githubusercontent.com/19966907/82762245-7cf1db80-9e32-11ea-9c0c-5e711e6ff5f9.png) ![screenshot-file-transfer](https://user-images.githubusercontent.com/19966907/82762322-e1149f80-9e32-11ea-80dc-762adcbcac48.png)


## Features

 * The communication goes «directly over i2p» from client to client, no server is required.[1] Note: The connections to the so-called «seedless» servers were removed from this project ; see : https://github.com/hypnosis-i2p/i2pchat/issues/40 . —hypn.

## How to run it

You need to enable SAM in your router on <a href="http://127.0.0.1:7657/configclients">java i2p configclients page</a> or i2pd's i2pd.conf [sam] section to make I2P Chat work over your I2P router.

## Project status, news and history

### Project status

Now the development was resumed by Hypnosis-i2p and R4SAS, with a lot more folks as testers &amp; bugs+issues reporters.

### Current news

5 Jan, 2017: Original repo at http://git.repo.i2p/w/I2P-Messenger-QT.git was fully merged here.

### History

Original developer of this messenger went away.

## License

The license of this software is GPLv2.

### Build instructions

 * Install prerequisites:
```
sudo apt-get install qt5-qmake qt5-default build-essential libqt5multimedia5 qtmultimedia5-dev
```
 * Run qmake for either release:
```
qmake I2P-Messenger.pro "CONFIG += release"
```
or for debug:
```
qmake I2P-Messenger.pro "CONFIG += debug"
```
 * And after that,
```
make -j NUMBER_OF_PROCESSOR_CORES
```
or simply
```
make
```

## Running

On Linux, `make` creates `I2P-Messenger` executable in the current folder. Run it with `./I2P-Messenger`. When ran, switch yourself to online. It will generate your Destination address (a key) on first connect to SAM.
