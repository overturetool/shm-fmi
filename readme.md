# Environment setup

Protobuf must be compiled and the path be avaliable in the environemnt variable `SHM_DEPENDENCIES_ROOT`

You can use the script `build-dependencies.sh` to compile protobuf

```bash
mkdir -p ~/dependencies
cd ~/dependencies

REPO/build-dependencies.sh linux32 <<<y
REPO/build-dependencies.sh linux64<<<y
REPO/build-dependencies.sh win32<<<y
REPO/build-dependencies.sh win64<<<y
REPO/build-dependencies.sh darwin64<<<y
```

and then in `~/.bash_profile` set

```bash
export SHM_DEPENDENCIES_ROOT=~/dependencies
```

# Compilation

./xcompile.sh 
