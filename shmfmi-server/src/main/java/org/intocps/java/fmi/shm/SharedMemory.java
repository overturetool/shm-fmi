package org.intocps.java.fmi.shm;

import java.io.File;
import java.lang.reflect.UndeclaredThrowableException;

import org.intocps.fmi.jnifmuapi.NativeLibraryLoader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SharedMemory implements ISharedMemory
{

	public final static String DEFAULT_MEMORY_NAME = "shmFmiTest";

	private final static String LIBRARY_NAME = "sharedmemory";




	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#setId(java.lang.String)
	 */
	@Override
	public native boolean setId(String id);

	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#read(byte[])
	 */
	@Override
	public native byte[] read(byte[] type);

	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#send(int, byte[])
	 */
	@Override
	public native void send(int type, byte[] data);
	
	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#setDebug(boolean)
	 */
	@Override
	public native void setDebug(boolean on);
	
	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#waitForWatchDogEvent()
	 */
	@Override
	public native void waitForWatchDogEvent();
	
	/* (non-Javadoc)
	 * @see org.intocps.java.fmi.shm.ISharedMemory#stop()
	 */
	@Override
	public native void stop();

	@Override public int getAliveInterval()
	{
		//native cycle is set to 200 ms
		return 300;
	}

	final static Logger logger = LoggerFactory.getLogger(SharedMemory.class);
	
	static boolean libraryLoaded = false;

	static
	{
		String libPath = NativeFmuApi.getPlatformSpecificLibaryPath(LIBRARY_NAME);
		logger.debug("Loading library: " + libPath);
		File lib;
		try
		{
			lib = new NativeLibraryLoader().loadLibrary(libPath);
			logger.debug("Loaded Library file: " + lib);
			libraryLoaded = true;
		} catch (Exception e)
		{
			logger.error("Faild to load native Library file: " + libPath, e);
			throw new UndeclaredThrowableException(e);
		}

	}

	static class NativeFmuApi
	{

		final static String LIB_BASE = "lib";

		public static String getPlatformSpecificLibaryPath(String name)
		{
			String osName = System.getProperty("os.name");

			int index = osName.indexOf(' ');
			if (index != -1)
			{
				osName = osName.substring(0, index);
			}

			String libPath = LIB_BASE + "/" + osName + "-"
					+ System.getProperty("os.arch") + "/" + name;
			// String libPath = LIB_BASE;

			// boolean x64 = System.getProperty("os.arch").contains("64");
			//
			if (osName.equalsIgnoreCase("windows"))
			{
				// if (x64)
				// {
				// libPath += "win64/";
				// } else
				// libPath += "win32/";
				libPath += ".dll";
				//
				return libPath;
			} else if (osName.equalsIgnoreCase("linux"))
			{
				// libPath="lib"+libPath+ ".so";
				libPath = LIB_BASE + "/" + osName + "-"
						+ System.getProperty("os.arch") + "/lib" + name + ".so";

				return libPath;
			} else if (osName.equalsIgnoreCase("Mac"))
			{
				// libPath="lib"+libPath+ ".so";
				// libPath += ".dylib";
				libPath = LIB_BASE + "/" + osName + "-"
						+ System.getProperty("os.arch") + "/lib" + name
						+ ".dylib";

				return libPath;
			}
			return null;
		}

	}

}
