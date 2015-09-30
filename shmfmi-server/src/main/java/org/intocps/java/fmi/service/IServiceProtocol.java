package org.intocps.java.fmi.service;

import com.google.protobuf.GeneratedMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.DoStepRequest;
import com.lausdahl.examples.Service.Empty;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.GetBooleanReply;
import com.lausdahl.examples.Service.GetIntegerReply;
import com.lausdahl.examples.Service.GetMaxStepSizeReply;
import com.lausdahl.examples.Service.GetRealReply;
import com.lausdahl.examples.Service.GetRequest;
import com.lausdahl.examples.Service.InstantiateRequest;
import com.lausdahl.examples.Service.SetBooleanRequest;
import com.lausdahl.examples.Service.SetDebugLoggingRequest;
import com.lausdahl.examples.Service.SetIntegerRequest;
import com.lausdahl.examples.Service.SetRealRequest;
import com.lausdahl.examples.Service.SetStringRequest;
import com.lausdahl.examples.Service.SetupExperimentRequest;

public interface IServiceProtocol {

	void error(String string);

	Fmi2StatusReply DoStep(DoStepRequest parseFrom);

	Fmi2StatusReply Terminate(Empty parseFrom);

	Fmi2StatusReply EnterInitializationMode(Empty parseFrom);

	Fmi2StatusReply ExitInitializationMode(Empty parseFrom);

	GetBooleanReply GetBoolean(GetRequest parseFrom);

	GetIntegerReply GetInteger(GetRequest parseFrom);

	GetMaxStepSizeReply GetMaxStepSize(Empty parseFrom);

	GetRealReply GetReal(GetRequest parseFrom);

	GeneratedMessage GetString(GetRequest parseFrom);

	Fmi2StatusReply Instantiate(InstantiateRequest parseFrom);

	Fmi2StatusReply Reset(Empty parseFrom);

	Fmi2StatusReply SetBoolean(SetBooleanRequest parseFrom);

	Fmi2StatusReply SetDebugLogging(SetDebugLoggingRequest parseFrom);

	Fmi2StatusReply SetInteger(SetIntegerRequest parseFrom);

	Fmi2StatusReply SetReal(SetRealRequest parseFrom);

	Fmi2StatusReply SetString(SetStringRequest parseFrom);

	Fmi2StatusReply SetupExperiment(SetupExperimentRequest parseFrom);

	void error(InvalidProtocolBufferException e);

}
