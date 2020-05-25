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
package org.intocps.fmi.jnifmuapi;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class NativeLibraryLoader
{
	final static Logger logger = LoggerFactory.getLogger(NativeLibraryLoader.class);

	static Map<String, File> loadedLibs = new HashMap<String, File>();

	public static File getTempFolder()
	{
		TempDirectory tempDir = new TempDirectory("jnifmuapi");
		tempDir.deleteOnExit();
		return tempDir.getPath().toFile();
	}

	public File loadLibrary(String relativeLibraryPath) throws Exception
	{
		if (loadedLibs.containsKey(relativeLibraryPath))
		{
			return loadedLibs.get(relativeLibraryPath);
		}

		logger.debug("Extracting library from: {}", relativeLibraryPath);
		File lib = extractNativeLibrary(getTempFolder(), relativeLibraryPath);
		logger.debug("Extracted library: {}", lib);

		if (lib == null)
		{
			throw new FileNotFoundException("Unable to find the native lib: "
					+ relativeLibraryPath);
		}

		System.load(lib.getAbsolutePath());

		loadedLibs.put(relativeLibraryPath, lib);

		return lib;
	}

	public File extractNativeLibrary(File outputFolder,
			String relativeLibraryPath) throws IOException
	{
		String name = relativeLibraryPath;

		int slashIndex = name.lastIndexOf('/');

		if (slashIndex != -1)
		{
			name = name.substring(slashIndex + 1);
		}

		String osName = System.getProperty("os.name");

		int index = osName.indexOf(' ');
		if (index != -1)
		{
			osName = osName.substring(0, index);
		}

		outputFolder = new File(outputFolder, "fmi-shm-" + osName + "-"
				+ System.getProperty("os.arch"));
		outputFolder.mkdirs();

		File library = new File(outputFolder, name);

		InputStream in = this.getClass().getClassLoader().getResourceAsStream(relativeLibraryPath);
		if (in != null)
		{
			Files.copy(in, Paths.get(library.getAbsolutePath()), StandardCopyOption.REPLACE_EXISTING);

			return library;
		}
		return null;
	}
}
