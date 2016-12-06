package org.intocps.java.fmi.shm;

public interface ISharedMemory
{

	public abstract boolean setId(String id);

	public abstract byte[] read(byte[] type);

	public abstract void send(int type, byte[] data);

	public abstract void setDebug(boolean on);

	public abstract void waitForWatchDogEvent();

	public abstract void stop();

	public int getAliveInterval();

}