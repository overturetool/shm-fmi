def copyStashInPlace ='''#!/bin/bash

echo Copy native libraries in place

LIB=shmfmi-server/target/classes/lib

mkdir -p $LIB/Mac-x86_64
mkdir -p $LIB/Linux-amd64
mkdir -p $LIB/Linux-x86
mkdir -p $LIB/Windows-amd64
mkdir -p $LIB/Windows-x86

echo Copying server libs

cp git-info.txt $LIB/
cp build/darwin64/shmfmi-server/libsharedmemory.dylib $LIB/Mac-x86_64/
cp build/linux64/shmfmi-server/libsharedmemory.so $LIB/Linux-amd64/
cp build/linux32/shmfmi-server/libsharedmemory.so $LIB/Linux-x86/
cp build/win64/shmfmi-server/sharedmemory.dll $LIB/Windows-amd64/
cp build/win32/shmfmi-server/sharedmemory.dll $LIB/Windows-x86/

# tag server
INFO=shmfmi-server/target/classes/git-info-server.txt
rm -f $INFO

set +e
git remote -v > $INFO
git show-ref HEAD >> $INFO
git rev-parse --verify HEAD >> $INFO
set -e

echo Copy FMU libs


FMU=vdm-tool-wrapper
FMUBIN=$FMU/binaries
mkdir -p $FMUBIN/darwin64
mkdir -p $FMUBIN/linux64
mkdir -p $FMUBIN/linux32
mkdir -p $FMUBIN/win64
mkdir -p $FMUBIN/win32

cp git-info.txt $FMUBIN/
cp build/darwin64/libshmfmu/liblibshmfmu.dylib $FMUBIN/darwin64/$FMU.dylib
cp build/linux64/libshmfmu/liblibshmfmu.so $FMUBIN/linux64/$FMU.so
cp build/linux32/libshmfmu/liblibshmfmu.so $FMUBIN/linux32/$FMU.so
cp build/win64/libshmfmu/libshmfmu.dll $FMUBIN/win64/$FMU.dll
cp build/win32/libshmfmu/libshmfmu.dll $FMUBIN/win32/$FMU.dll

cp -r $FMU $LIB
'''


stage('Compile native code X') {
			
	parallel  darwin: {
		node('Xcompile') {
			checkout scm
			sh 'git submodule update --init'
			lock('SHM-FMI-TEST'){
				sh "SHM_DEPENDENCIES_ROOT=/home/privatejenkins/shm-dependencies ./xcompile.sh -darwin"
			}
			sh "./list-dependencies.sh"
			stash includes: '**/*.dylib, git-info.txt, **/gtestresults.xml', name: 'jni-fmi-native-api-darwin' 
		}
	},linux32: {
		node('Xcompile') {
			checkout scm
			sh 'git submodule update --init'
			lock('SHM-FMI-TEST'){
				sh "SHM_DEPENDENCIES_ROOT=/home/privatejenkins/shm-dependencies ./xcompile.sh -linux32"
			}
			sh "./list-dependencies.sh"
			stash includes: '**/*.so, git-info.txt, **/gtestresults.xml', name: 'jni-fmi-native-api-linux32' 
		}
	},linux64: {
		node('Xcompile') {
			checkout scm
			sh 'git submodule update --init'
			lock('SHM-FMI-TEST'){
				sh "SHM_DEPENDENCIES_ROOT=/home/privatejenkins/shm-dependencies ./xcompile.sh -linux64"
			}
			sh "./list-dependencies.sh"
			stash includes: '**/*.so, git-info.txt, shmfmi-server/target/generated-protoc/**/*.java, **/gtestresults.xml', name: 'jni-fmi-native-api-linux64'  
		}
	},win32: {
		node('Xcompile') {
			checkout scm
			sh 'git submodule update --init'
			lock('SHM-FMI-TEST'){
				sh "SHM_DEPENDENCIES_ROOT=/home/privatejenkins/shm-dependencies ./xcompile.sh -win32"
			}
			sh "./list-dependencies.sh"
			stash includes: '**/*.dll, git-info.txt, **/gtestresults.xml', name: 'jni-fmi-native-api-win32' 
		}
	},win64: {
		node('Xcompile') {
			checkout scm
			sh 'git submodule update --init'
			lock('SHM-FMI-TEST'){
				sh "SHM_DEPENDENCIES_ROOT=/home/privatejenkins/shm-dependencies ./xcompile.sh -win64"
			}
			sh "./list-dependencies.sh"
			stash includes: '**/*.dll, git-info.txt, **/gtestresults.xml', name: 'jni-fmi-native-api-win64'
		}
	}
}

node {

	// Only keep one build
	properties([[$class: 'BuildDiscarderProperty', strategy: [$class: 'LogRotator', numToKeepStr: '5']]])

	stage('Compile Java library') {
		checkout scm

		//clear all
		withMaven(mavenLocalRepo: '.repository', mavenSettingsFilePath: "${env.MVN_SETTINGS_PATH}") {

			// Run the maven build
			sh "mvn clean -f shmfmi-server/pom.xml"
				
		}
		sh "rm -r jnifmuapi/src/main/native/build"

		//copy back all native libraries
		unstash 'jni-fmi-native-api-darwin' 
		unstash 'jni-fmi-native-api-linux32'
		unstash 'jni-fmi-native-api-linux64'
		unstash 'jni-fmi-native-api-win32'
		unstash 'jni-fmi-native-api-win64'
							
		sh copyStashInPlace

		//compile the java library
		withMaven(mavenLocalRepo: '.repository', mavenSettingsFilePath: "${env.MVN_SETTINGS_PATH}") {

			// Run the maven build
			sh "mvn install -f shmfmi-server/pom.xml -P!protoc"
			step([$class: 'ArtifactArchiver', artifacts: '**/target/*.jar', fingerprint: true])
			step([$class: 'JUnitResultArchiver', testResults: '**/target/surefire-reports/TEST-*.xml, **/gtestresults.xml'])
			step([$class: 'JacocoPublisher'])
			step([$class: 'TasksPublisher', canComputeNew: false, defaultEncoding: '', excludePattern: '', healthy: '', high: 'FIXME', ignoreCase: true, low: '', normal: 'TODO', pattern: '', unHealthy: ''])
		}

	}

	stage('Deploy') {
		if (env.BRANCH_NAME == 'development') {
			
			def server = Artifactory.server "-225816141@1439286524510"
			def buildInfo = Artifactory.newBuildInfo()
			buildInfo.env.capture = true
				
			def rtMaven = Artifactory.newMavenBuild()
			rtMaven.tool = "Maven 3.3.3" // Tool name from Jenkins configuration
			rtMaven.opts = "-Xmx1024m -XX:MaxPermSize=256M"
			rtMaven.deployer releaseRepo:'into-cps', snapshotRepo:'into-cps', server: server
				
			rtMaven.run pom: 'shmfmi-server/pom.xml', goals: 'install', buildInfo: buildInfo

			//get rid of old snapshots only keep then for a short amount of time
			buildInfo.retention maxBuilds: 5, maxDays: 7, deleteBuildArtifacts: true
		
			// Publish build info.
			server.publishBuildInfo buildInfo
		}
	}
	stage('Build COE') {
		build 'overture-fmu-pipe/development'
	}
}


