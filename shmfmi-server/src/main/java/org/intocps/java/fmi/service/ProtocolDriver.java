package org.intocps.java.fmi.service;

import org.intocps.java.fmi.shm.SharedMemory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.*;

public class ProtocolDriver implements Runnable {

	final static Logger logger = LoggerFactory.getLogger(ProtocolDriver.class);
	final IServiceProtocol service;
	final Thread thread;
	final SharedMemory mem;
	boolean running = true;

	public ProtocolDriver(String id, IServiceProtocol service) {

		this.mem = new SharedMemory();
		
		int retries = 10;
		boolean connected = false;
		
		System.out.println("Starting shared memory with key: "+id);
		logger.debug("Starting shared memory with key: {}",id);
		while (!(connected=this.mem.setId(id)) && retries>=0) {
			try {
				Thread.sleep(500);
				retries--;
			} catch (InterruptedException e) {
				// don't care
			}
		}
		
		if(!connected)
		{
			throw new RuntimeException("Unable to connect");
		}
		
		this.service = service;

		thread = new Thread(this);
		thread.setDaemon(true);
	}

	public void start() {
		thread.start();
	}

	@Override
	public void run() {

		byte[] typeArr = new byte[1];

		while (running) {

			byte[] bytes = this.mem.read(typeArr);
			if (bytes == null)
				continue;// timout

			byte type = typeArr[0];

			Commands cmd = Commands.lookup(type);
			
			logger.debug("Recieved command: {}",cmd);
			
			if (cmd == null)
				service.error("Unknown type");

			com.google.protobuf.GeneratedMessage reply = null;

			try {

				switch (cmd) {
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
					break;
				case fmi2Instantiate:
					reply = service.Instantiate(Fmi2InstantiateRequest.parseFrom(bytes));
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
					
					//status
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

			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				service.error(e);
				logger.error("Decode error, returning FATAL message no matter what return type was expectede", e);
				//This may be the wrong reply but better than nothing
				reply=Fmi2StatusReply.newBuilder().setStatus(Fmi2StatusReply.Status.Fatal).build();
			}

			if (reply != null){
				logger.debug("Sending message type {}",type);
				this.mem.send(type, reply.toByteArray());
			}else {
				service.error("deadlocking");
				logger.error("deadlocked");
			}
		}

	}

}
