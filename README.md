# lmaocat
This program is a clone of [lolcat](https://github.com/busyloop/lolcat), it takes input from a file or from the standard input and makes it rainbow-colored.
- It's over 10x faster, because lmaocat is written in C++.
- lmaocat, unlike other clones, calculates the colors the same way
  the original tool did it, which leads to cleaner-looking colors.
- The only required dependencies of lmaocat are a C++ compiler and the
  C++ standard library, even though the installation is easier with cmake installed
- lmaocat doesn't have 8-bit color support, it will just print out the normal 
  text. This is because truecolor is supported pretty much everywhere nowadays.

![Could not load Image!](./assets/help.png "lmaocat --help")

## Installation
Installing dependencies [git, cmake, g++ / clang] (for example on debian based distros):
```shell
sudo apt install git cmake build-essential
```

On pretty much every GNU/Linux distro lmaocat can be installed with:
```shell
git clone https://github.com/CyberWolfTV/lmaocat.git

mkdir lmaocat/build
cd lmaocat/build
cmake ..
make

command -v doas && alias sudo="$(command -v doas)"

sudo chown root lmaocat
sudo mv lmaocat /usr/bin/lmaocat
sudo ln -s /usr/bin/lmaocat /usr/bin/lolcat

cd ../..
rm -rf lmaocat

clear

lmaocat --help
```


## Disclaimer
- This project is not related to [lolcat](https://github.com/busyloop/lolcat) in any way, it's just an independent clone.
- I'm not responsible for any harm or damage this tool causes.
- For more information see LICENSE
