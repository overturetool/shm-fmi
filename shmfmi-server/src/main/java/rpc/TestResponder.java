package rpc;

import java.util.HashMap;
import java.util.Map;

import org.intocps.java.fmi.service.IServiceProtocol;

import com.google.protobuf.GeneratedMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.DoStepRequest;
import com.lausdahl.examples.Service.Empty;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.Fmi2StatusReply.Status;
import com.lausdahl.examples.Service.GetStringReply.Builder;
import com.lausdahl.examples.Service.GetBooleanReply;
import com.lausdahl.examples.Service.GetIntegerReply;
import com.lausdahl.examples.Service.GetMaxStepSizeReply;
import com.lausdahl.examples.Service.GetRealReply;
import com.lausdahl.examples.Service.GetRequest;
import com.lausdahl.examples.Service.GetStringReply;
import com.lausdahl.examples.Service.InstantiateRequest;
import com.lausdahl.examples.Service.SetBooleanRequest;
import com.lausdahl.examples.Service.SetDebugLoggingRequest;
import com.lausdahl.examples.Service.SetIntegerRequest;
import com.lausdahl.examples.Service.SetRealRequest;
import com.lausdahl.examples.Service.SetStringRequest;
import com.lausdahl.examples.Service.SetupExperimentRequest;

public class TestResponder implements IServiceProtocol{

	Map<Integer, Double> reals = new HashMap<Integer, Double>();
	Map<Integer, Boolean> booleans = new HashMap<Integer, Boolean>();
	Map<Integer, Integer> integers = new HashMap<Integer, Integer>();
	Map<Integer, String> strings = new HashMap<Integer, String>();
	
	public TestResponder() {
		reals.put(10, 9.9);
		integers.put(11, 1);
		booleans.put(12, true);
		strings.put(13, "undefined");
	}
	
	static Fmi2StatusReply ok(){
		return Fmi2StatusReply.newBuilder().setStatus(Status.Ok).build();
	}
	
	
	@Override
	public void error(String string) {
		System.err.println(string);
	}

	@Override
	public Fmi2StatusReply DoStep(DoStepRequest parseFrom) {
		System.out.println("Called fmi2DoStep");
		return ok();
	}

	@Override
	public Fmi2StatusReply Terminate(Empty parseFrom) {
		System.out.println("Called fmi2Terminate");
		return ok();
	}

	@Override
	public Fmi2StatusReply EnterInitializationMode(Empty parseFrom) {
		System.out.println("Called fmi2EnterInitializationMode");
		return ok();
	}

	@Override
	public Fmi2StatusReply ExitInitializationMode(Empty parseFrom) {
		System.out.println("Called fmi2ExitInitializationMode");
		return ok();
	}

	@Override
	public GetBooleanReply GetBoolean(GetRequest parseFrom) {
		System.out.println("Called fmi2GetBoolean");

		com.lausdahl.examples.Service.GetBooleanReply.Builder builder = GetBooleanReply
				.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList()) {
			if (booleans.containsKey(id))
				builder.addValues(booleans.get(id));
		}

		return (builder.build());
	}

	@Override
	public GetIntegerReply GetInteger(GetRequest parseFrom) {
		System.out.println("Called fmi2GetInteger");

		com.lausdahl.examples.Service.GetIntegerReply.Builder builder = GetIntegerReply
				.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList()) {
			if (integers.containsKey(id))
				builder.addValues(integers.get(id));
		}

		return builder.build();
	}

	@Override
	public GetMaxStepSizeReply GetMaxStepSize(Empty parseFrom) {
		System.out.println("Called fmi2GetMaxStepSize");
		
		GetMaxStepSizeReply reply =GetMaxStepSizeReply.newBuilder().setMaxStepSize(100).build();
		return (reply);
	}

	@Override
	public GetRealReply GetReal(GetRequest parseFrom) {
		System.out.println("Called fmi2GetReal");

		com.lausdahl.examples.Service.GetRealReply.Builder builder = GetRealReply
				.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList()) {
			if (reals.containsKey(id))
				builder.addValues(reals.get(id));
		}

		return builder.build();
	}

	@Override
	public GetStringReply GetString(GetRequest parseFrom) {
		System.out.println("Called fmi2GetString");

		Builder builder = GetStringReply.newBuilder();

		for (Integer id : parseFrom.getValueReferenceList()) {
			if (strings.containsKey(id))
				builder.addValues(strings.get(id));
		}

		return (builder.build());
	}

	@Override
	public Fmi2StatusReply Instantiate(InstantiateRequest parseFrom) {
		System.out.println("Called fmi2Instantiate");
		return ok();
	}

	@Override
	public Fmi2StatusReply Reset(Empty parseFrom) {
		System.out.println("Called fmi2Reset");
		return ok();
	}

	@Override
	public Fmi2StatusReply SetBoolean(SetBooleanRequest request) {
		System.out.println("Called fmi2SetBoolean");

		for (int i = 0; i < request.getValueReferenceCount(); i++) {
			booleans.put(request.getValueReference(i), request.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetDebugLogging(SetDebugLoggingRequest parseFrom) {
		System.out.println("Called fmi2SetDebugLogging");
		return ok();
	}

	@Override
	public Fmi2StatusReply SetInteger(SetIntegerRequest request) {
		System.out.println("Called fmi2SetInteger");

		for (int i = 0; i < request.getValueReferenceCount(); i++) {
			integers.put(request.getValueReference(i), request.getValues(i));
		}
return ok();
	}

	@Override
	public Fmi2StatusReply SetReal(SetRealRequest parseFrom) {
		System.out.println("Called fmi2SetReal");

		for (int i = 0; i < parseFrom.getValueReferenceCount(); i++) {
			reals.put(parseFrom.getValueReference(i), parseFrom.getValues(i));
		}
return ok();
	}

	@Override
	public Fmi2StatusReply SetString(SetStringRequest request) {
		System.out.println("Called fmi2SetString");

		for (int i = 0; i < request.getValueReferenceCount(); i++) {
			strings.put(request.getValueReference(i), request.getValues(i));
			System.out.println("\t" + request.getValues(i));
		}
		return ok();
	}

	@Override
	public Fmi2StatusReply SetupExperiment(SetupExperimentRequest parseFrom) {
		System.out.println("Called fmi2SetupExperiment");
		return ok();
	}

	@Override
	public void error(InvalidProtocolBufferException e) {
		e.printStackTrace();
	}

}
