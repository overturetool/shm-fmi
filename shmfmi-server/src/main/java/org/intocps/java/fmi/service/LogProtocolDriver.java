package org.intocps.java.fmi.service;

import org.intocps.java.fmi.shm.SharedMemoryServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.lausdahl.examples.Service.Fmi2StringStatusReply;

public class LogProtocolDriver
{

	final static Logger logger = LoggerFactory.getLogger(LogProtocolDriver.class);

	final SharedMemoryServer mem;

	public LogProtocolDriver(String id)
	{

		this.mem = new SharedMemoryServer();

		int retries = 10;
		boolean connected = false;

		System.out.println("Starting shared memory with key: " + id);
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
			throw new RuntimeException("Unable to connect");
		}

	}

	public void log(String message)
	{
		logger.debug("Sending callback log message: {}",message);
		Fmi2StringStatusReply msg = Fmi2StringStatusReply.newBuilder().setValue(message).build();
		mem.serverSend(Commands.fmi2Log.id, msg.toByteArray());
	}

}