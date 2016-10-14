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

```bash
./xcompile.sh 
```

## Release the library


```bash
cd shmfmi-server
mvn -Dmaven.repo.local=repository release:clean
mvn -Dmaven.repo.local=repository release:prepare -DreleaseVersion=${RELEASE_VER} -DdevelopmentVersion=${NEW_DEV_VER}
mvn -Dmaven.repo.local=repository release:perform
```

This will build the new (java) release and upload it to artifactory. However, it will not include the platform specific libraries required. This is just not automated!!!

## Patch the release with the platform specific libraries

Go to Artifactory https://overture.au.dk/artifactory/webapp/browserepo.html?2

Navigate to:

* into-cps/org/fmi
   * fmi-shm-server
        * X.X.X-SNAPSHOT
            * `fmi-shm-server-X.X.X.*.jar`   <-- Download this file. It is the latest development version which contains the libraries which is missing in the release
        * Y.Y.Y
            * `fmi-shm-server-Y.Y.Y.*.jar`   <-- Download this file. It is the released jar

`X.X.X` is last development version and `Y.Y.Y` is the release of `X.X.X`.

copy the `fmi-shm-server-X.X.X.*.jar/lib` into `fmi-shm-server-Y.Y.Y.*.jar` and redeploy the release jar to Artifactory.
