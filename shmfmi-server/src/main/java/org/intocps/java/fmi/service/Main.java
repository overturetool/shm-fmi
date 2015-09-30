package org.intocps.java.fmi.service;

import rpc.TestResponder;

public class Main {

	public static void main(String[] args) throws InterruptedException {
		System.loadLibrary("sharedmemory");
		// System.load("C:/Users/kel/workspace/sharedmemoryTestClient/Debug/libsharedmemoryTestClient.dll");

		ProtocolDriver driver = new ProtocolDriver("Local\\MyFileMappingObject2", new TestResponder());

		driver.start();

		while (true)
			Thread.sleep(30000);

	}

}
