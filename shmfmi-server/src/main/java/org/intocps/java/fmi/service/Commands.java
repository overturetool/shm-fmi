package org.intocps.java.fmi.service;

/**
 * Converted manually from SharedFmiMessage.h
 * @author kel
 *
 */
public enum Commands {
	fmi2SetDebugLogging(0),
	fmi2Instantiate(1),
//	fmi2FreeInstance,
	fmi2SetupExperiment(2),
	fmi2EnterInitializationMode(3),
	fmi2ExitInitializationMode(4),
	fmi2Terminate(5),
	fmi2Reset(6),
	fmi2DoStep(7),

	fmi2GetReal(8),
	fmi2GetInteger(9),
	fmi2GetBoolean(10),
	fmi2GetString(11),

	fmi2SetReal(12),
	fmi2SetInteger(13),
	fmi2SetBoolean(14),
	fmi2SetString(15),

	fmi2GetMaxStepSize(16);
	
	public final byte id;
	private Commands(int id) {
		this.id = (byte)id;
	}
	
	
	public static Commands lookup(byte id)
	{
		for (Commands cmd : values()) {
			if(cmd.id==id)
				return cmd;
		}
		return null;
	}
}
