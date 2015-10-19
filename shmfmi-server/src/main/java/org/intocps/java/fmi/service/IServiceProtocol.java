package org.intocps.java.fmi.service;

import com.google.protobuf.GeneratedMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.*;

public interface IServiceProtocol {

	void error(String string);

	Fmi2StatusReply DoStep(Fmi2DoStepRequest parseFrom);

	Fmi2StatusReply Terminate(Fmi2Empty parseFrom);

	Fmi2StatusReply EnterInitializationMode(Fmi2Empty parseFrom);

	Fmi2StatusReply ExitInitializationMode(Fmi2Empty parseFrom);

	Fmi2GetBooleanReply GetBoolean(Fmi2GetRequest parseFrom);

	Fmi2GetIntegerReply GetInteger(Fmi2GetRequest parseFrom);

	Fmi2GetMaxStepSizeReply GetMaxStepSize(Fmi2Empty parseFrom);

	Fmi2GetRealReply GetReal(Fmi2GetRequest parseFrom);

	GeneratedMessage GetString(Fmi2GetRequest parseFrom);

	Fmi2StatusReply Instantiate(Fmi2InstantiateRequest parseFrom);

	Fmi2StatusReply Reset(Fmi2Empty parseFrom);

	Fmi2StatusReply SetBoolean(Fmi2SetBooleanRequest parseFrom);

	Fmi2StatusReply SetDebugLogging(Fmi2SetDebugLoggingRequest parseFrom);

	Fmi2StatusReply SetInteger(Fmi2SetIntegerRequest parseFrom);

	Fmi2StatusReply SetReal(Fmi2SetRealRequest parseFrom);

	Fmi2StatusReply SetString(Fmi2SetStringRequest parseFrom);

	Fmi2StatusReply SetupExperiment(Fmi2SetupExperimentRequest parseFrom);

	void error(InvalidProtocolBufferException e);

}
