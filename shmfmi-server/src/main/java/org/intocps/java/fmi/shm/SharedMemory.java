package org.intocps.java.fmi.shm;

import com.google.protobuf.InvalidProtocolBufferException;

public class SharedMemory {

public	native void setId(String id);

public	native byte[] read(byte[] type);

public native void send(int type, byte[] data);

	public static void main(String[] args) throws InvalidProtocolBufferException {

		System.loadLibrary("sharedmemory");
		// System.load("C:/Users/kel/workspace/sharedmemoryTestClient/Debug/libsharedmemoryTestClient.dll");

		SharedMemory mem = new SharedMemory();
		mem.setId("Local\\MyFileMappingObject2");

		/*
		byte[] type = new byte[1];
		while (true) {
			byte[] data = mem.read(type);
			//System.out.println(String.format("Read type (%s) payload: %s", type[0] + "", data + ""));

			if (data != null) {
				Fmi2StatusReply reply = Fmi2StatusReply.parseFrom(data);
				System.out.println("parsed message: "+reply.getStatus());
				
			}
		}
		*/

	}
}
