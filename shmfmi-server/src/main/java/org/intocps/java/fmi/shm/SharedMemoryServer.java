package org.intocps.java.fmi.shm;

import java.io.File;
import java.lang.reflect.UndeclaredThrowableException;

import org.intocps.fmi.jnifmuapi.NativeLibraryLoader;
import org.intocps.java.fmi.shm.SharedMemory.NativeFmuApi;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SharedMemoryServer
{

	public final static String DEFAULT_MEMORY_NAME = "shmFmiTestCallback";

	private final static String LIBRARY_NAME = "sharedmemory";

	public native boolean serverStart(String id);
	
	public native void serverStop();

	public static native void setServerDebug(boolean on);

	public native void serverSend(int type, byte[] data);

	final static Logger logger = LoggerFactory.getLogger(SharedMemoryServer.class);

	static
	{
		if (!SharedMemory.libraryLoaded)
		{
			String libPath = NativeFmuApi.getPlatformSpecificLibaryPath(LIBRARY_NAME);
			logger.debug("Loading library: " + libPath);
			File lib;
			try
			{
				lib = new NativeLibraryLoader().loadLibrary(libPath);
				logger.debug("Loaded Library file: " + lib);
			} catch (Exception e)
			{
				logger.error("Faild to load native Library file: " + libPath, e);
				throw new UndeclaredThrowableException(e);
			}
		}
	}

}
