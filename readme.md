# SHared Memory FMI

[![Build Status](https://build.overture.au.dk/jenkins/buildStatus/icon?job=overture-development)](https://build.overture.au.dk/jenkins/job$)

[![License](http://img.shields.io/:license-gpl3-blue.svg?style=flat-square)](http://www.gnu.org/licenses/gpl-3.0.html)

[![Maven Central](https://img.shields.io/maven-central/v/org.overturetool/core.svg?label=Maven%20Central)](http://search.maven.org/#search$)


# Environment setup

You must have the following installed to use the cross compilation:
* docker
* java
* maven 3



If you want to install the development env locally then check the scripts for you plantform under `scripts`

If will consist of three parts:

1. Compile protobuf
2. Run cmake and build the native library
3. Make `protoc` available in `shmfmi-server`
    e.g. `ln -s <path to the build version of protobuf>` called from the folder `shmfmi-server`

# Compilation

```bash
./xcompile.sh 
mvn install -f shmfmi-server/pom.xml
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

Go to Artifactory http://overture.au.dk/artifactory/simple/into-cps/org/intocps/java/fmi/fmi-shm-server/

Navigate to:

* into-cps/org/fmi
   * fmi-shm-server
        * X.X.X-SNAPSHOT
            * `fmi-shm-server-X.X.X.*.jar`   <-- Download this file. It is the latest development version which contains the libraries which is missing in the release
        * Y.Y.Y
            * `fmi-shm-server-Y.Y.Y.*.jar`   <-- Download this file. It is the released jar

`X.X.X` is last development version and `Y.Y.Y` is the release of `X.X.X`.

copy the `fmi-shm-server-X.X.X.*.jar/lib` into `fmi-shm-server-Y.Y.Y.*.jar` and redeploy the release jar to Artifactory.
