
node {

	// Only keep one build
	properties([[$class: 'BuildDiscarderProperty', strategy: [$class: 'LogRotator', numToKeepStr: '5']]])

	stage('Compile Java library') {
	    cleanWs()

		checkout scm
		sh 'git submodule update --init'

		//clear all
		withMaven(mavenLocalRepo: '.repository', mavenSettingsFilePath: "${env.MVN_SETTINGS_PATH}") {


			// Run the maven build
			sh "mvn clean -f shmfmi-server/pom.xml"
				
		}



		//compile the java library
		withMaven(mavenLocalRepo: '.repository', mavenSettingsFilePath: "${env.MVN_SETTINGS_PATH}") {

            sh "./xcompile.sh"
            sh "cd shmfmi-server && cp ../third_party/protobuf/builds/mac/release/bin/protoc ."


			// Run the maven build
			sh "mvn install -f shmfmi-server/pom.xml"
			step([$class: 'ArtifactArchiver', artifacts: '**/target/*.jar', fingerprint: true])
			step([$class: 'JUnitResultArchiver', testResults: '**/target/surefire-reports/TEST-*.xml, **/gtestresults.xml'])
			step([$class: 'JacocoPublisher'])
			step([$class: 'TasksPublisher', canComputeNew: false, defaultEncoding: '', excludePattern: '', healthy: '', high: 'FIXME', ignoreCase: true, low: '', normal: 'TODO', pattern: '', unHealthy: ''])
		}

	}

	stage('Deploy') {
		if (env.BRANCH_NAME == 'development') {
			
			def server = Artifactory.server "-844406945@1404457436085"
			def buildInfo = Artifactory.newBuildInfo()
			buildInfo.env.capture = true
				
			def rtMaven = Artifactory.newMavenBuild()
			rtMaven.tool = "Maven 3.1.1" // Tool name from Jenkins configuration
			rtMaven.opts = "-Xmx1024m -XX:MaxPermSize=256M"
			rtMaven.deployer releaseRepo:'into-cps', snapshotRepo:'into-cps', server: server
				
			rtMaven.run pom: 'shmfmi-server/pom.xml', goals: 'install -P!protoc', buildInfo: buildInfo

			//get rid of old snapshots only keep then for a short amount of time
			//buildInfo.retention maxBuilds: 5, maxDays: 7, deleteBuildArtifacts: true
		
			// Publish build info.
			server.publishBuildInfo buildInfo
		}
	}
	stage('Build Overture-FMU') {
		build job: 'overture-fmu/development', wait: false
	}
}


