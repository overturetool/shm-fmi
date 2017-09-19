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

import com.google.protobuf.InvalidProtocolBufferException;
import com.lausdahl.examples.Service.Fmi2BooleanStatusReply;
import com.lausdahl.examples.Service.Fmi2DoStepRequest;
import com.lausdahl.examples.Service.Fmi2Empty;
import com.lausdahl.examples.Service.Fmi2GetBooleanReply;
import com.lausdahl.examples.Service.Fmi2GetIntegerReply;
import com.lausdahl.examples.Service.Fmi2GetMaxStepSizeReply;
import com.lausdahl.examples.Service.Fmi2GetRealReply;
import com.lausdahl.examples.Service.Fmi2GetRequest;
import com.lausdahl.examples.Service.Fmi2GetStringReply;
import com.lausdahl.examples.Service.Fmi2InstantiateRequest;
import com.lausdahl.examples.Service.Fmi2IntegerStatusReply;
import com.lausdahl.examples.Service.Fmi2RealStatusReply;
import com.lausdahl.examples.Service.Fmi2SetBooleanRequest;
import com.lausdahl.examples.Service.Fmi2SetDebugLoggingRequest;
import com.lausdahl.examples.Service.Fmi2SetIntegerRequest;
import com.lausdahl.examples.Service.Fmi2SetRealRequest;
import com.lausdahl.examples.Service.Fmi2SetStringRequest;
import com.lausdahl.examples.Service.Fmi2SetupExperimentRequest;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.Fmi2StatusRequest;
import com.lausdahl.examples.Service.Fmi2StringStatusReply;

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

	Fmi2GetStringReply GetString(Fmi2GetRequest parseFrom);

	Fmi2StatusReply Instantiate(Fmi2InstantiateRequest parseFrom);

	void FreeInstantiate(Fmi2Empty parseFrom);

	Fmi2StatusReply Reset(Fmi2Empty parseFrom);

	Fmi2StatusReply SetBoolean(Fmi2SetBooleanRequest parseFrom);

	Fmi2StatusReply SetDebugLogging(Fmi2SetDebugLoggingRequest parseFrom);

	Fmi2StatusReply SetInteger(Fmi2SetIntegerRequest parseFrom);

	Fmi2StatusReply SetReal(Fmi2SetRealRequest parseFrom);

	Fmi2StatusReply SetString(Fmi2SetStringRequest parseFrom);

	Fmi2StatusReply SetupExperiment(Fmi2SetupExperimentRequest parseFrom);
	
	Fmi2StatusReply GetStatus(Fmi2StatusRequest request);
	Fmi2RealStatusReply GetRealStatus(Fmi2StatusRequest request);
	Fmi2IntegerStatusReply GetIntegerStatus(Fmi2StatusRequest request);
	Fmi2BooleanStatusReply GetBooleanStatus(Fmi2StatusRequest request);
	Fmi2StringStatusReply GetStringStatus(Fmi2StatusRequest request);

	void error(InvalidProtocolBufferException e);


}
