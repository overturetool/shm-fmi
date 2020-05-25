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

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.intocps.java.fmi.shm.ISharedMemory;
import org.intocps.java.fmi.shm.SharedMemory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.Fmi2DoStepRequest;
import com.lausdahl.examples.Service.Fmi2Empty;
import com.lausdahl.examples.Service.Fmi2GetRequest;
import com.lausdahl.examples.Service.Fmi2GetStringReply;
import com.lausdahl.examples.Service.Fmi2InstantiateRequest;
import com.lausdahl.examples.Service.Fmi2SetBooleanRequest;
import com.lausdahl.examples.Service.Fmi2SetDebugLoggingRequest;
import com.lausdahl.examples.Service.Fmi2SetIntegerRequest;
import com.lausdahl.examples.Service.Fmi2SetRealRequest;
import com.lausdahl.examples.Service.Fmi2SetStringRequest;
import com.lausdahl.examples.Service.Fmi2SetupExperimentRequest;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.Fmi2StatusRequest;

public class ProtocolDriver implements Runnable
{

	final static Logger logger = LoggerFactory.getLogger(ProtocolDriver.class);
	final IServiceProtocol service;
	final Thread thread;
	final ISharedMemory mem;
	final String id;
	boolean connected = false;

	public ProtocolDriver(String id, IServiceProtocol service)
	{
		this(id, service, new SharedMemory());
	}

	public ProtocolDriver(String id, IServiceProtocol service, ISharedMemory mem)
	{
		this.mem = mem;
		this.service = service;
		this.id = id;
		thread = new Thread(this);
		thread.setDaemon(true);
	}

	public void open()
	{
		if (!connected)
		{
			int retries = 10;
			connected = false;

			logger.debug("Starting shared memory with key: {}", id);
			while (!(connected = this.mem.setId(id)) && retries >= 0)
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

			connected = true;

			Thread watchDog = new Thread(new Runnable()
			{

				@Override
				public void run()
				{
					setWatchDog();
				}
			});
			watchDog.setDaemon(true);
			watchDog.start();
			thread.start();
		}
	}

	private void handleTimeout()
	{
		final String timeOutMessage = "No alive signal from FMU withing %d ms period. Freeing instance!";
		String msg = String.format(timeOutMessage, mem.getAliveInterval());
		System.err.println(msg);
		logger.warn(msg);
		connected = false;
		service.FreeInstantiate(Fmi2Empty.newBuilder().build());
	}

	private void setWatchDog()
	{
		final ExecutorService executor = Executors.newCachedThreadPool();
		final Callable<Boolean> task = new Callable<Boolean>()
		{
			public Boolean call()
			{
				return mem.waitForWatchDogEvent();
			}
		};

		while (connected)
		{
			Future<Boolean> future = executor.submit(task);
			try
			{
				if (!future.get(mem.getAliveInterval(), TimeUnit.MILLISECONDS))
				{
					handleTimeout();
					return;
				}
			} catch (TimeoutException ex)
			{
				// handle the timeout
				handleTimeout();
				return;
			} catch (InterruptedException e)
			{
				// handle the interrupts
			} catch (ExecutionException e)
			{
				// handle other exceptions
			} finally
			{
				future.cancel(true); // may or may not desire this
			}
		}
	}

	public void close()
	{
		if (!connected)
		{
			return;
		}
		logger.debug("Stopping Shared memory client");
		connected = false;
		mem.stop();
	}

	@Override
	public void run()
	{
		final int maxBufferSize = mem.getBufferSize();

		byte[] typeArr = new byte[1];

		while (connected)
		{

			byte[] bytes = this.mem.read(typeArr);
			if (bytes == null)
			{
				try
				{
					Thread.sleep(100);
				} catch (InterruptedException e)
				{
				}
				continue;
			}

			byte type = typeArr[0];

			Commands cmd = Commands.lookup(type);

			logger.debug("Recieved command: {}", cmd);

			if (cmd == null)
			{
				service.error("Unknown type");
			}

			com.google.protobuf.GeneratedMessageV3 reply = null;

			try
			{

				switch (cmd)
				{
					case fmi2DoStep:
						reply = service.DoStep(Fmi2DoStepRequest.parseFrom(bytes));
						break;
					case fmi2EnterInitializationMode:
						reply = service.EnterInitializationMode(Fmi2Empty.parseFrom(bytes));
						break;
					case fmi2ExitInitializationMode:
						reply = service.ExitInitializationMode(Fmi2Empty.parseFrom(bytes));
						break;
					case fmi2GetBoolean:
						reply = service.GetBoolean(Fmi2GetRequest.parseFrom(bytes));
						break;
					case fmi2GetInteger:
						reply = service.GetInteger(Fmi2GetRequest.parseFrom(bytes));
						break;
					case fmi2GetMaxStepSize:
						reply = service.GetMaxStepSize(Fmi2Empty.parseFrom(bytes));
						break;
					case fmi2GetReal:
						reply = service.GetReal(Fmi2GetRequest.parseFrom(bytes));
						break;
					case fmi2GetString:
						reply = service.GetString(Fmi2GetRequest.parseFrom(bytes));
						if(reply.getSerializedSize() > maxBufferSize)
						{
							service.error("Buffer overflow. Protobuf message too big. Actual: "+reply.getSerializedSize()+" max allowed: "+maxBufferSize);
							logger.error("Buffer overflow. Protobuf message too big. Actual: "+reply.getSerializedSize()+" max allowed: "+maxBufferSize);
							reply = Fmi2GetStringReply.newBuilder().setValid(false).build();
						}
						break;
					case fmi2Instantiate:
						reply = service.Instantiate(Fmi2InstantiateRequest.parseFrom(bytes));
						break;
					case fmi2FreeInstance:
						service.FreeInstantiate(Fmi2Empty.parseFrom(bytes));
						reply = null; // no reply
						break;
					case fmi2Reset:
						reply = service.Reset(Fmi2Empty.parseFrom(bytes));
						break;
					case fmi2SetBoolean:
						reply = service.SetBoolean(Fmi2SetBooleanRequest.parseFrom(bytes));
						break;
					case fmi2SetDebugLogging:
						reply = service.SetDebugLogging(Fmi2SetDebugLoggingRequest.parseFrom(bytes));
						break;
					case fmi2SetInteger:
						reply = service.SetInteger(Fmi2SetIntegerRequest.parseFrom(bytes));
						break;
					case fmi2SetReal:
						reply = service.SetReal(Fmi2SetRealRequest.parseFrom(bytes));
						break;
					case fmi2SetString:
						reply = service.SetString(Fmi2SetStringRequest.parseFrom(bytes));
						break;
					case fmi2SetupExperiment:
						reply = service.SetupExperiment(Fmi2SetupExperimentRequest.parseFrom(bytes));
						break;
					case fmi2Terminate:
						reply = service.Terminate(Fmi2Empty.parseFrom(bytes));
						break;

					// status
					case fmi2GetStatus:
						reply = service.GetStatus(Fmi2StatusRequest.parseFrom(bytes));
						break;
					case fmi2GetRealStatus:
						reply = service.GetRealStatus(Fmi2StatusRequest.parseFrom(bytes));
						break;
					case fmi2GetIntegerStatus:
						reply = service.GetIntegerStatus(Fmi2StatusRequest.parseFrom(bytes));
						break;
					case fmi2GetBooleanStatus:
						reply = service.GetBooleanStatus(Fmi2StatusRequest.parseFrom(bytes));
						break;
					case fmi2GetStringStatus:
						reply = service.GetStringStatus(Fmi2StatusRequest.parseFrom(bytes));
						break;

					default:

						break;

				}

			} catch (InvalidProtocolBufferException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
				service.error(e);
				logger.error("Decode error, returning FATAL message no matter what return type was expectede", e);
				// This may be the wrong reply but better than nothing
				reply = Fmi2StatusReply.newBuilder().setStatus(Fmi2StatusReply.Status.Fatal).build();
			}

			if (reply != null)
			{
				logger.debug("Sending message type {}", type);

				bytes = reply.toByteArray();
				if (bytes.length > maxBufferSize)
				{
					InvalidProtocolBufferException e =new InvalidProtocolBufferException("Buffer overflow. Protobuf message too big. Actual: "+bytes.length+" max allowed: "+maxBufferSize); 
					service.error(e);
					logger.error(e.getMessage()+" Returning FATAL", e);
					// This may be the wrong reply but better than nothing
					//reply = Fmi2StatusReply.newBuilder().setStatus(Fmi2StatusReply.Status.Fatal).build();
					this.mem.send(type, new byte[]{});
				} else{

				this.mem.send(type, reply.toByteArray());
				}
			} else
			{
				String errorMsg = "deadlocking do not know how to answer: "
						+ cmd;
				service.error(errorMsg);
				logger.error(errorMsg);
			}
		}

	}

}
