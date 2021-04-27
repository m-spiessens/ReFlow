# Blinky using Flow

## Setup Development Environment

```
sudo apt update && sudo apt install python3-pip git git-lfs g++-9 cmake
sudo pip3 install --upgrade pip && pip install --user conan
conan config install https://gitlab.com/m-spiessens/conan-config.git
```

## Build

```
conan install . -pr:h TM4C129ENCPDT -if=../EK-TM4C129EXL-Debug/ -s build_type=Debug
conan build . -bf=../EK-TM4C129EXL-Debug/
```

### Visual Studio Code

Open folder and press `Ctrl+Shift+b`.
See `.vscode/tasks.json`.

### Eclipse

Import existing project from `EK-TM4C129EXL-Debug` directory.