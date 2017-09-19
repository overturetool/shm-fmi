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