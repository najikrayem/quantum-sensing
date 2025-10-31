# quantum-sensing

## Software Prerequisites (Ubuntu Host)
Download and install the following software if you don't already have them to be able to compile the code.
1. git
2. CMake
3. Python3
4. Native Compiler
5. GCC Cross compiler
6. Baremetal C library implementations

```
sudo apt update
sudo apt upgrade
sudo apt install git cmake python3 build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

You also need the Visual Studio code IDE with the PlatformIO extension installed.

- https://code.visualstudio.com/download

- https://platformio.org/


## Project Setup
1. If you have not already done so, configure Git locally with your GitHub account.

   - First, add your email and username:
     ```bash
     git config --global user.name "Your Name"
     git config --global user.email "you@example.com"
     ```

   - Generate the SSH key:
     ```bash
     ssh-keygen -t ed25519 -C "you@example.com"
     eval "$(ssh-agent -s)"
     ssh-add ~/.ssh/id_ed25519
     ```

   - Add the SSH key to your GitHub account:
     1. Copy the key from your computer:
        ```bash
        cat ~/.ssh/id_ed25519.pub
        ```
     2. Go to [**GitHub → Settings → SSH and GPG Keys**](https://github.com/settings/keys) and paste the key there.

2. Clone the project locally 
 ``` bash
 git clone https://github.com/najikrayem/quantum-sensing.git
 ```

3. Initialize Submodules. This will install things like the PICO-SDK submodule which is necessary for the RP2350 board.
``` bash
git submodule init
```