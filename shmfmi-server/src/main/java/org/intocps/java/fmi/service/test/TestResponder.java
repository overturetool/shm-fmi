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
package org.intocps.java.fmi.service.test;

import java.util.HashMap;
import java.util.Map;

import org.intocps.java.fmi.service.IServiceProtocol;
import org.intocps.java.fmi.service.LogProtocolDriver;
import org.intocps.java.fmi.service.ProtocolDriver;

import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.Fmi2BooleanStatusReply;
import com.lausdahl.examples.Service.Fmi2DoStepRequest;
import com.lausdahl.examples.Service.Fmi2Empty;
import com.lausdahl.examples.Service.Fmi2IntegerStatusReply;
import com.lausdahl.examples.Service.Fmi2LogReply;
import com.lausdahl.examples.Service.Fmi2RealStatusReply;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.Fmi2StatusReply.Status;
import com.lausdahl.examples.Service.Fmi2GetBooleanReply;
import com.lausdahl.examples.Service.Fmi2GetIntegerReply;
import com.lausdahl.examples.Service.Fmi2GetMaxStepSizeReply;
import com.lausdahl.examples.Service.Fmi2GetRealReply;
import com.lausdahl.examples.Service.Fmi2GetRequest;
import com.lausdahl.examples.Service.Fmi2GetStringReply;
import com.lausdahl.examples.Service.Fmi2GetStringReply.Builder;
import com.lausdahl.examples.Service.Fmi2InstantiateRequest;
import com.lausdahl.examples.Service.Fmi2SetBooleanRequest;
import com.lausdahl.examples.Service.Fmi2SetDebugLoggingRequest;
import com.lausdahl.examples.Service.Fmi2SetIntegerRequest;
import com.lausdahl.examples.Service.Fmi2SetRealRequest;
import com.lausdahl.examples.Service.Fmi2SetStringRequest;
import com.lausdahl.examples.Service.Fmi2SetupExperimentRequest;
import com.lausdahl.examples.Service.Fmi2StatusRequest;
import com.lausdahl.examples.Service.Fmi2StringStatusReply;

public class TestResponder implements IServiceProtocol
{

	public static void main(String[] args) throws InterruptedException
	{

		String memoryKey = "shmFmiTest";// "OvertureFmiFileMappingObject";
		if (args.length > 0)
		{
			memoryKey = args[0];
		}

		ProtocolDriver driver = new ProtocolDriver(memoryKey, new TestResponder());

		driver.open();

		while (true)
			Thread.sleep(30000);

	}

	Map<Integer, Double> reals = new HashMap<Integer, Double>();
	Map<Integer, Boolean> booleans = new HashMap<Integer, Boolean>();
	Map<Integer, Integer> integers = new HashMap<Integer, Integer>();
	Map<Integer, String> strings = new HashMap<Integer, String>();

	public TestResponder()
	{
		reals.put(10, 9.9);
		integers.put(11, 1);
		booleans.put(12, true);
		strings.put(13, "undefined");
	}

	static Fmi2StatusReply ok()
	{
		return Fmi2StatusReply.newBuilder().setStatus(Status.Ok).build();
	}

	@Override
	public void error(String string)
	{
		System.err.println(string);
	}

	@Override
	public Fmi2StatusReply DoStep(Fmi2DoStepRequest parseFrom)
	{
		System.out.println("Called fmi2DoStep");
		return ok();
	}

	@Override
	public Fmi2StatusReply Terminate(Fmi2Empty parseFrom)
	{
		System.out.println("Called fmi2Terminate");
		return ok();
	}

	@Override
	public Fmi2StatusReply EnterInitializationMode(Fmi2Empty parseFrom)
	{
		System.out.println("Called fmi2EnterInitializationMode");
		if (logDriver != null)
		{
			logDriver.log("logAll", Fmi2LogReply.Status.Ok, "Called fmi2EnterInitializationMode");
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply ExitInitializationMode(Fmi2Empty parseFrom)
	{
		System.out.println("Called fmi2ExitInitializationMode");
		return ok();
	}

	@Override
	public Fmi2GetBooleanReply GetBoolean(Fmi2GetRequest parseFrom)
	{
		System.out.println("Called fmi2GetBoolean");

		Fmi2GetBooleanReply.Builder builder = Fmi2GetBooleanReply.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList())
		{
			if (booleans.containsKey(id))
				builder.addValues(booleans.get(id));
		}

		return (builder.build());
	}

	@Override
	public Fmi2GetIntegerReply GetInteger(Fmi2GetRequest parseFrom)
	{
		System.out.println("Called fmi2GetInteger");

		Fmi2GetIntegerReply.Builder builder = Fmi2GetIntegerReply.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList())
		{
			if (integers.containsKey(id))
				builder.addValues(integers.get(id));
		}

		return builder.build();
	}

	@Override
	public Fmi2GetMaxStepSizeReply GetMaxStepSize(Fmi2Empty parseFrom)
	{
		System.out.println("Called fmi2GetMaxStepSize");

		Fmi2GetMaxStepSizeReply reply = Fmi2GetMaxStepSizeReply.newBuilder().setMaxStepSize(100).build();
		return (reply);
	}

	@Override
	public Fmi2GetRealReply GetReal(Fmi2GetRequest parseFrom)
	{
		System.out.println("Called fmi2GetReal");

		Fmi2GetRealReply.Builder builder = Fmi2GetRealReply.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList())
		{
			if (reals.containsKey(id))
				builder.addValues(reals.get(id));
		}

		return builder.build();
	}

	@Override
	public Fmi2GetStringReply GetString(Fmi2GetRequest parseFrom)
	{
		System.out.println("Called fmi2GetString");

		Builder builder = Fmi2GetStringReply.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList())
		{
			if (strings.containsKey(id))
				builder.addValues(strings.get(id));
		}

		return (builder.build());
	}

	LogProtocolDriver logDriver = null;

	@Override
	public Fmi2StatusReply Instantiate(Fmi2InstantiateRequest parseFrom)
	{
		System.out.println("Called fmi2Instantiate");

		if (parseFrom.getLogginOn())
		{
			logDriver = new LogProtocolDriver(parseFrom.getCallbackShmName());

		}

		return ok();
	}

	@Override
	public Fmi2StatusReply Reset(Fmi2Empty parseFrom)
	{
		System.out.println("Called fmi2Reset");
		return ok();
	}

	@Override
	public Fmi2StatusReply SetBoolean(Fmi2SetBooleanRequest request)
	{
		System.out.println("Called fmi2SetBoolean");

		for (int i = 0; i < request.getValueReferenceCount(); i++)
		{
			booleans.put(request.getValueReference(i), request.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetDebugLogging(Fmi2SetDebugLoggingRequest parseFrom)
	{
		System.out.println("Called fmi2SetDebugLogging");
		return ok();
	}

	@Override
	public Fmi2StatusReply SetInteger(Fmi2SetIntegerRequest request)
	{
		System.out.println("Called fmi2SetInteger");

		for (int i = 0; i < request.getValueReferenceCount(); i++)
		{
			integers.put(request.getValueReference(i), request.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetReal(Fmi2SetRealRequest parseFrom)
	{
		System.out.println("Called fmi2SetReal");

		for (int i = 0; i < parseFrom.getValueReferenceCount(); i++)
		{
			reals.put(parseFrom.getValueReference(i), parseFrom.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetString(Fmi2SetStringRequest request)
	{
		System.out.println("Called fmi2SetString");

		for (int i = 0; i < request.getValueReferenceCount(); i++)
		{
			strings.put(request.getValueReference(i), request.getValues(i));
			System.out.println("\t" + request.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetupExperiment(Fmi2SetupExperimentRequest parseFrom)
	{
		System.out.println("Called fmi2SetupExperiment");
		return ok();
	}

	@Override
	public void error(InvalidProtocolBufferException e)
	{
		e.printStackTrace();
	}

	@Override
	public Fmi2StatusReply GetStatus(Fmi2StatusRequest request)
	{
		return ok();
	}

	@Override
	public Fmi2RealStatusReply GetRealStatus(Fmi2StatusRequest request)
	{
		return Fmi2RealStatusReply.newBuilder().setValue(100.5).build();
	}

	@Override
	public Fmi2IntegerStatusReply GetIntegerStatus(Fmi2StatusRequest request)
	{
		return Fmi2IntegerStatusReply.newBuilder().setValue(100).build();
	}

	@Override
	public Fmi2BooleanStatusReply GetBooleanStatus(Fmi2StatusRequest request)
	{
		if (request.getStatus() == Fmi2StatusRequest.FmiStatusKind.fmi2Terminated)
			return Fmi2BooleanStatusReply.newBuilder().setValue(true).build();
		else
			return Fmi2BooleanStatusReply.newBuilder().setValue(false).build();

	}

	@Override
	public Fmi2StringStatusReply GetStringStatus(Fmi2StatusRequest request)
	{
		if (request.getStatus() == Fmi2StatusRequest.FmiStatusKind.fmi2DoStepStatus)
			return Fmi2StringStatusReply.newBuilder().setValue("waiting").build();
		else
			return Fmi2StringStatusReply.newBuilder().setValue("nothing").build();
	}

	@Override
	public void FreeInstantiate(Fmi2Empty parseFrom)
	{
	}

}
