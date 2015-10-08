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
	final static String JAVA_IO_TEMP_PROPERTY = "java.io.tmpdir";

	static Map<String, File> loadedLibs = new HashMap<String, File>();

	public static File getTempFolder()
	{
		return new File(System.getProperty(JAVA_IO_TEMP_PROPERTY));
	}

	public File loadLibrary(String relativeLibraryPath) throws Exception
	{
		if (loadedLibs.containsKey(relativeLibraryPath))
		{
			return loadedLibs.get(relativeLibraryPath);
		}

		logger.debug("Extracting library from: {}",relativeLibraryPath);
		File lib = extractNativeLibrary(getTempFolder(), relativeLibraryPath);
		
		if(lib==null)
		{
			throw new FileNotFoundException("Unable to find the native lib: "+relativeLibraryPath);
		}

		String libname = lib.getName();

		int libnameDotIndex = libname.indexOf('.');

		if (libnameDotIndex != -1)
		{
			libname = libname.substring(0, libnameDotIndex);
		}
		
		if(libname.startsWith("lib")&& (System.getProperty("os.name").equalsIgnoreCase("linux")|| System.getProperty("os.name").toLowerCase().startsWith("mac")))
		{
			libname = libname.substring(3);
		}

		// Add the folder in which the library exists to the lib load path
		addLibraryPath(lib.getParentFile());

		System.loadLibrary(libname);

		loadedLibs.put(relativeLibraryPath, lib);

		return lib;
	}

	public File extractNativeLibrary(File outputFolder,
			String relativeLibraryPath)
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
		
		outputFolder = new File(outputFolder,"fmi-shm-" +osName + "-"
				+ System.getProperty("os.arch"));
		outputFolder.mkdirs();

		File library = new File(outputFolder, name );

		InputStream in = this.getClass().getClassLoader().getResourceAsStream(relativeLibraryPath);
		if (in != null)
		{
			try
			{
				Files.copy(in, Paths.get(library.getAbsolutePath()), StandardCopyOption.REPLACE_EXISTING);

				// request the file to be deleted on exit
				library.deleteOnExit();

				return library;
			} catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();

			}
		}
		return null;
	}

	/**
	 * Adds the specified path to the java library path
	 *
	 * @param pathToAdd
	 *            the path to add
	 * @throws Exception
	 */
	public static void addLibraryPath(File dirPathToAdd) throws Exception
	{
		final String pathToAdd = dirPathToAdd.getAbsolutePath();
		final Field usrPathsField = ClassLoader.class.getDeclaredField("usr_paths");
		usrPathsField.setAccessible(true);

		// get array of paths
		final String[] paths = (String[]) usrPathsField.get(null);

		// check if the path to add is already present
		for (String path : paths)
		{
			if (path.equals(pathToAdd))
			{
				return;
			}
		}

		// add the new path
		final String[] newPaths = Arrays.copyOf(paths, paths.length + 1);
		newPaths[newPaths.length - 1] = pathToAdd;
		usrPathsField.set(null, newPaths);
	}

	// public static void Load(File jarFile, File destDir, String lib)
	// throws IOException
	// {
	// JarFile jar = new JarFile(jarFile);
	// Enumeration<?> enumEntries = jar.entries();
	// while (enumEntries.hasMoreElements())
	// {
	// JarEntry file = (JarEntry) enumEntries.nextElement();
	// File f = new File(destDir, file.getName());
	// if (file.isDirectory())
	// { // if its a directory, create it
	// f.mkdir();
	// continue;
	// }
	// InputStream is = jar.getInputStream(file); // get the input stream
	// FileOutputStream fos = new FileOutputStream(f);
	// while (is.available() > 0)
	// { // write contents of 'is' to 'fos'
	// fos.write(is.read());
	// }
	// fos.close();
	// is.close();
	// }
	// }
}