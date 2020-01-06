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
package org.intocps.java.fmi.service;

import org.intocps.java.fmi.shm.SharedMemoryServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.lausdahl.examples.Service.Fmi2LogReply;
import com.lausdahl.examples.Service.Fmi2LogReply.Status;

public class LogProtocolDriver
{

	final static Logger logger = LoggerFactory.getLogger(LogProtocolDriver.class);

	final SharedMemoryServer mem;
	boolean connected = false;

	public LogProtocolDriver(String id)
	{

		this.mem = new SharedMemoryServer();

		int retries = 10;
		connected = false;

		logger.debug("Starting shared memory with key: {}", id);
		while (!(connected = this.mem.serverStart(id)) && retries >= 0)
		{
			try
			{
				Thread.sleep(500);
				retries--;
			} catch (InterruptedException e)
			{
				// don't care
			}
		}

		if (!connected)
		{
			logger.error("Starting shared memory with key: {}. -- Failed to connect, giving up, now", id);
			throw new RuntimeException("Unable to connect");
		}
		logger.debug("Starting shared memory with key: {}. Completed and connected", id);
		connected = true;
	}

	public void log(String category, Status status, String message)
	{
		logger.debug("Calling log. Is connected: {}", connected);
		if (!connected)
		{
			return;
		}
		logger.debug("Sending callback log message: {}", message);
		Fmi2LogReply msg = Fmi2LogReply.newBuilder().setValue(message).setCategory(category).setStatus(status).build();
		mem.serverSend(Commands.fmi2Log.id, msg.toByteArray());
	}

	public void close()
	{
		if (!connected)
		{
			return;
		}
		logger.debug("Stopping callback Shared memory server");
		mem.serverStop();
	}

}