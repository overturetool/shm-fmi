/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */

#include "org_intocps_java_fmi_shm_SharedMemory.h"
#include "IpcClient.h"

FmiIpc::IpcClient* g_client;
bool g_clientDebug = true;


JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_setDebug(JNIEnv *env, jobject obj, jboolean on)
{
	g_clientDebug = on;
}

JNIEXPORT jboolean JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_setId(JNIEnv *env, jobject obj, jstring id)
{

	const char * idString = env->GetStringUTFChars(id, NULL);
	if (g_clientDebug)
	{
		printf("Native g_client (java) starting g_client with shm key: %s\n", idString);
		fflush(stdout);
	}
	bool success;

	g_client = new FmiIpc::IpcClient(0, idString);
	if (g_clientDebug)
	{
		g_client->enableConsoleDebug();
	}

	g_client->connect(&success);

	if (!success)
	{
		delete g_client;
		g_client = NULL;
	}

	env->ReleaseStringUTFChars(id, idString);

	return success;

}


JNIEXPORT jboolean JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_waitForWatchDogEvent(JNIEnv *env, jobject obj)
{

	if (g_client != NULL)
	{
		g_client->waitForWatchDogEvent();
		return true;
	}
	return false;
}

/*
 * Class:     sharedmemoryTestg_client_SharedMemory
 * Method:    read
 * Signature: ([B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_read(JNIEnv *env, jobject obj, jbyteArray types)
{

	SharedFmiMessage* msg = g_client->getMessage(5);

	if (msg == NULL)
	{
		return NULL;
	}

	jbyte *vbody = env->GetByteArrayElements(types, 0);

	vbody[0] = msg->cmd;

	env->ReleaseByteArrayElements(types, vbody, 0);

	long size = (long) msg->protoBufMsgSize;

	jbyteArray result;
	result = env->NewByteArray(size);
	if (result == NULL)
	{
		return NULL; /* out of memory error thrown */
	}

	jbyte fill[size];

	for (int i = 0; i < size; i++)
	{
		fill[i] = msg->protoBufMsg[i];
	}

	env->SetByteArrayRegion(result, 0, size, fill);

	return result;
}

JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_send(JNIEnv *env, jobject obj, jint type,
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

	g_client->sendReply(msg);
	delete msg;

}

JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_stop(JNIEnv *env, jobject obj)
{
	delete g_client;
	g_client = NULL;
}

JNIEXPORT jint JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_getBufferSize
  (JNIEnv *env, jobject obj){
	return SharedFmiBufferSize;
}
