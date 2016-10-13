package org.intocps.fmi.jnifmuapi;

import java.io.IOException;
import java.nio.file.FileVisitResult;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.attribute.BasicFileAttributes;

public class TempDirectory
{
	final Path path;

	public TempDirectory(String prefix)
	{
		try
		{
			path = Files.createTempDirectory(prefix);
		} catch (IOException e)
		{
			throw new RuntimeException(e);
		}
	}

	public Path getPath()
	{
		return path;
	}

	public void deleteOnExit()
	{
		Runtime.getRuntime().addShutdownHook(new Thread()
		{
			@Override
			public void run()
			{
				delete(path);
			}
		});
	}

	private static void deleteOrScheduleOnExit(final Path path)
	{
		try
		{
			Files.deleteIfExists(path);
		} catch (IOException e)
		{
			try
			{
				Runtime.getRuntime().addShutdownHook(new Thread()
				{
					@Override
					public void run()
					{
						try
						{
							delete(path);
						} catch (Exception e)
						{
							// we cannot not do anything about this
						}
					}
				});
			} catch (IllegalStateException es)
			{
				// ignore
			}
		}
	}

	public static void delete(final Path path)
	{
		if (!Files.exists(path))
		{
			return;
		}
		try
		{
			Files.walkFileTree(path, new SimpleFileVisitor<Path>()
			{
				@Override
				public FileVisitResult postVisitDirectory(Path dir,
						IOException exc) throws IOException
				{
					deleteOrScheduleOnExit(dir);
					return super.postVisitDirectory(dir, exc);
				}

				@Override
				public FileVisitResult visitFile(Path file,
						BasicFileAttributes attrs) throws IOException
				{
					deleteOrScheduleOnExit(file);
					return super.visitFile(file, attrs);
				}
			});
		} catch (IOException e)
		{
			throw new RuntimeException(e);
		}
	}

}