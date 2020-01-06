/*
 * #%~
 * Java SHM-FMI server
 * %%
 * Copyright (C) 2015 - 2017 Overture
 * %%
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
 * #~%
 */
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
	
	public native int getServerBufferSize();

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
