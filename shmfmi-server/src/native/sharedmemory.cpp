/*
 * sharedmemory.cpp
 *
 *  Created on: 28/09/2015
 *      Author: kel
 */

#include "org_intocps_java_fmi_shm_SharedMemory.h"
#include "FmiIpc.h"

FmiIpc::Client* client;

JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_setId(
		JNIEnv *env, jobject obj, jstring id) {

	const char * idString = env->GetStringUTFChars(id, NULL);
	printf("%s\n",idString);


	client = new FmiIpc::Client("Local\\MyFileMappingObject2");

}

/*
 * Class:     sharedmemoryTestClient_SharedMemory
 * Method:    read
 * Signature: ([B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_read(
		JNIEnv *env, jobject obj, jbyteArray types) {

	SharedFmiMessage* msg = client->getMessage(INFINITE);

	if (msg == NULL) {
		return NULL;
	}

	jbyte *vbody = env->GetByteArrayElements(types, 0);

		vbody[0] = msg->cmd;


	env->ReleaseByteArrayElements(types, vbody, 0);

	long size = (long) msg->protoBufMsgSize;

	jbyteArray result;
	result = env->NewByteArray(size);
	if (result == NULL) {
		return NULL; /* out of memory error thrown */
	}

	jbyte fill[size];

	//printf("############## Type: %i Size: %i\n",msg->cmd,size);

	printf("Reading: ");
	for (int i = 0; i < size; i++) {
		fill[i] = msg->protoBufMsg[i];
		//printf("%u",msg->protoBufMsg[i]);
	}
	//printf("\n");

	env->SetByteArrayRegion(result, 0, size, fill);

	return result;
}



JNIEXPORT void JNICALL Java_org_intocps_java_fmi_shm_SharedMemory_send(
		JNIEnv *env, jobject obj, jint type, jbyteArray bytes) {

	size_t size = env->GetArrayLength(bytes);
	SharedFmiMessage* msg = new SharedFmiMessage();

	msg->protoBufMsgSize = size;
	msg->cmd =static_cast<fmi2Command>(type);

	jbyte *vbody = env->GetByteArrayElements(bytes, 0);
	for (int i = 0; i < size; i++) {
		msg->protoBufMsg[i] = vbody[i];
	}

	env->ReleaseByteArrayElements(bytes, vbody, 0);

	client->sendReply(msg);
	delete msg;

}
