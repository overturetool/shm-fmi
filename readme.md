# SHared Memory FMI

[![Build Status](https://build.overture.au.dk/jenkins/buildStatus/icon?job=overture-development)](https://build.overture.au.dk/jenkins/job$)

[![License](http://img.shields.io/:license-gpl3-blue.svg?style=flat-square)](http://www.gnu.org/licenses/gpl-3.0.html)

[![Maven Central](https://img.shields.io/maven-central/v/org.overturetool/core.svg?label=Maven%20Central)](http://search.maven.org/#search$)

# Project overview

The overview of the Overture-FMU is presented in this section.
Please see https://www.ncl.ac.uk/media/wwwnclacuk/schoolofcomputingscience/files/trs/1524.pdf#section*.9 for a related publication.

The COE and Overture-FMU communicated via IPC (shared memory) based on protobuf messages.

![Overture-FMU Architecture](readme-resources/overturefmu.png)

Libraries corresponding to project directories are marked with *italic*

*libshmfmu* acts as the FMU interface from the COE process.

## libshmfmu
- Starts an Overture-FMU process based on *overture-fmu/fmi-interpreter*.
- Implements the FMI interfacts and is the FMU interface from the COE process.
- Sends FMI commands to Overture-FMU via *libshmfmi*.
- Creates an IPC server for sending FMI commands via *libshmipc*.
- Connects as client to an IPC server for receiving callbacks via *libshmipc*

## overture-fmu/fmi-interpreter
- Invokes the overture interpreter to perform the actions required.
- Receives commands via *shmfmi-server*
- Replies via *shmfmi-server*

## libshmfmi
- Sends FMI commands via *libshmipc*
- Receives replies

## libshmipc
- Controls the shared memory both in terms of server and client.
- Handles access to shared memory via semaphores

## shmfmi-server 
- Java interface to *libshmipc*
- Creates an IPC server for sending callbacks via *libshmipc*
- Connects as client to an IPC server for receiving FMI commands via *libshmipc*

## Starting an Overture-FMU

The flow in loading an Overture-FMU in view of a co-simulation and the folders of the shm-fmi repository and the  overture-fmu repository is the following:

COE loads *libshmfmu*.
*libshmfmu* starts the java process *overture-fmu/fmi-interpreter*.

At this stage there are two processes related to overture-fmu that communicates via shared memory (IPC):
1. The COE
2. Overture-FMU 

- The COE communicates via *libshmfmu*
- The *Overture-FMU/fmu-interpreter* communicates via *shmfmi-server*.


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
