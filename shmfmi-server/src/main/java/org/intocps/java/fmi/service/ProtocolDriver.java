package org.intocps.java.fmi.service;

import org.intocps.java.fmi.shm.SharedMemory;

import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.DoStepRequest;
import com.lausdahl.examples.Service.Empty;
import com.lausdahl.examples.Service.GetRequest;
import com.lausdahl.examples.Service.InstantiateRequest;
import com.lausdahl.examples.Service.SetBooleanRequest;
import com.lausdahl.examples.Service.SetDebugLoggingRequest;
import com.lausdahl.examples.Service.SetIntegerRequest;
import com.lausdahl.examples.Service.SetRealRequest;
import com.lausdahl.examples.Service.SetStringRequest;
import com.lausdahl.examples.Service.SetupExperimentRequest;

public class ProtocolDriver implements Runnable {

	final IServiceProtocol service;
	final Thread thread;
	final SharedMemory mem;
	boolean running = true;

	public ProtocolDriver(String id, IServiceProtocol service) {

		this.mem = new SharedMemory();
		this.mem.setId(id);
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
			if (cmd == null)
				service.error("Unknown type");

			com.google.protobuf.GeneratedMessage reply = null;

			try {

				switch (cmd) {
				case fmi2DoStep:
					reply = service.DoStep(DoStepRequest.parseFrom(bytes));
					break;
				case fmi2EnterInitializationMode:
					reply = service.EnterInitializationMode(Empty.parseFrom(bytes));
					break;
				case fmi2ExitInitializationMode:
					reply = service.ExitInitializationMode(Empty.parseFrom(bytes));
					break;
				case fmi2GetBoolean:
					reply = service.GetBoolean(GetRequest.parseFrom(bytes));
					break;
				case fmi2GetInteger:
					reply = service.GetInteger(GetRequest.parseFrom(bytes));
					break;
				case fmi2GetMaxStepSize:
					reply = service.GetMaxStepSize(Empty.parseFrom(bytes));
					break;
				case fmi2GetReal:
					reply = service.GetReal(GetRequest.parseFrom(bytes));
					break;
				case fmi2GetString:
					reply = service.GetString(GetRequest.parseFrom(bytes));
					break;
				case fmi2Instantiate:
					reply = service.Instantiate(InstantiateRequest.parseFrom(bytes));
					break;
				case fmi2Reset:
					reply = service.Reset(Empty.parseFrom(bytes));
					break;
				case fmi2SetBoolean:
					reply = service.SetBoolean(SetBooleanRequest.parseFrom(bytes));
					break;
				case fmi2SetDebugLogging:
					reply = service.SetDebugLogging(SetDebugLoggingRequest.parseFrom(bytes));
					break;
				case fmi2SetInteger:
					reply = service.SetInteger(SetIntegerRequest.parseFrom(bytes));
					break;
				case fmi2SetReal:
					reply = service.SetReal(SetRealRequest.parseFrom(bytes));
					break;
				case fmi2SetString:
					reply = service.SetString(SetStringRequest.parseFrom(bytes));
					break;
				case fmi2SetupExperiment:
					reply = service.SetupExperiment(SetupExperimentRequest.parseFrom(bytes));
					break;
				case fmi2Terminate:
					reply = service.Terminate(Empty.parseFrom(bytes));
					break;
				default:

					break;

				}

			} catch (InvalidProtocolBufferException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				service.error(e);
			}

			if (reply != null)
				this.mem.send(type, reply.toByteArray());
			else {
				service.error("deadlocking");
			}
		}

	}

}
