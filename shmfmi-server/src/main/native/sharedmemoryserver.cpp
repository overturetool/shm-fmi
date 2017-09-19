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

#include "org_intocps_java_fmi_shm_SharedMemoryServer.h"
#include "IpcServer.h"

FmiIpc::IpcServer* g_server = NULL;
bool g_serverDebug = true;

JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_setServerDebug(JNIEnv *env, jclass clz,
		jboolean on)
{
	g_serverDebug = on;
}

JNIEXPORT jboolean JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_serverStart(JNIEnv *env, jobject obj,
		jstring id)
{
	const char * shmKey = env->GetStringUTFChars(id, NULL);

	if (g_serverDebug)
	{
		printf("\nNative Server: Starting callback g_server with key: %s\n", shmKey);
		fflush(stdout);
	}

	g_server = new FmiIpc::IpcServer(0, shmKey);
	if(g_serverDebug)
	{
		g_server->enableConsoleDebug();
	}

	bool success = g_server->create();
	if (!success)
	{
		if (g_serverDebug)
		{
			printf("Native Server: Unable to create callback g_server with key: %s\n", shmKey);
		}
		delete g_server;
		g_server = NULL;
	}

	env->ReleaseStringUTFChars(id, shmKey);
	if (g_serverDebug)
	{
		printf("Native Server: Create callback g_server with key: %s. Completed\n", shmKey);
		fflush(stdout);
	}
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

//	printf("Sending type: %i Size: %i\n",msg->cmd,msg->protoBufMsgSize);fflush(stdout);
	SharedFmiMessage* reply = g_server->send(msg, 20);
	delete msg;

	if (reply == NULL)
	{
//		printf("Sending type: %i Size: %i. -- TIMEOUT\n",msg->cmd,msg->protoBufMsgSize);fflush(stdout);
		return; //timeout
	}

}

JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_serverStop(JNIEnv *env, jobject obj)
{
	delete g_server;
	g_server = NULL;
}

JNIEXPORT jint JNICALL Java_org_intocps_java_fmi_shm_SharedMemoryServer_getServerBufferSize
  (JNIEnv *env, jobject obj){
	return SharedFmiBufferSize;
}
