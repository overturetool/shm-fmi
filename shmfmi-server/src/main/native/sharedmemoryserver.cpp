/*
 * sharedmemory.cpp
 *
 *  Created on: 28/09/2015
 *      Author: kel
 */

#include "org_intocps_java_fmi_shm_SharedMemoryServer.h"
#include "FmiIpc.h"

FmiIpc::Server* server;

JNIEXPORT jboolean JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_serverStart(JNIEnv *env, jobject obj, jstring id)
{
	fflush(stdout); //FIXME remove
	const char * shmKey = env->GetStringUTFChars(id, NULL);
	printf("\nNative Server: Starting callback server with key: %s\n", shmKey);
	fflush(stdout);

	server = new FmiIpc::Server();


	bool success = server->create(shmKey);
	if (!success)
	{
		printf("Native Server: Unable to create callback server with key: %s\n",shmKey);
		delete server;
		server = NULL;
	}

	env->ReleaseStringUTFChars(id, shmKey);
	fflush(stdout); //FIXME remove
	return success;

}


JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_serverSend(JNIEnv *env, jobject obj, jint type,
		jbyteArray bytes)
{

	size_t size = env->GetArrayLength(bytes);
	SharedFmiMessage* msg = new SharedFmiMessage();

	msg->protoBufMsgSize = size;
	msg->cmd = static_cast<fmi2Command>(type);

	jbyte *vbody = env->GetByteArrayElements(bytes, 0);
	for (int i = 0; i < size; i++)
	{
		msg->protoBufMsg[i] = vbody[i];
	}

	env->ReleaseByteArrayElements(bytes, vbody, 0);

	//printf("Sending type: %i Size: %i\n",m.cmd,m.protoBufMsgSize);
	SharedFmiMessage* reply = server->send(msg, INFINITE);
	delete msg;

	if (reply == NULL)
	{
		return ; //timeout
	}


}
