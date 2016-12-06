import static org.mockito.Matchers.anyString;
import static org.mockito.Mockito.*;

import org.intocps.java.fmi.service.IServiceProtocol;
import org.intocps.java.fmi.service.ProtocolDriver;
import org.intocps.java.fmi.shm.ISharedMemory;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Matchers;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;
import org.powermock.modules.junit4.PowerMockRunner;

import com.lausdahl.examples.Service.Fmi2Empty;

@RunWith(PowerMockRunner.class) public class TimeOutTest
{
	int cycles = 0;
	boolean timedOut = false;

	@Before public  void setup()
	{
		cycles = 0;
		timedOut = false;
	}

	@Test public void noTimeOutTest() throws InterruptedException
	{
		timeOutTest(100, 0);
		if (timedOut)
			Assert.fail("Timed out");
	}

	@Test public void timeOutTest() throws InterruptedException
	{
		timeOutTest(1, 100);
		if (!timedOut)
			Assert.fail("Missing timed out");
	}

	void timeOutTest(int expected, final int actual) throws InterruptedException
	{
		ISharedMemory mem = mock(ISharedMemory.class);
		when(mem.setId(anyString())).thenReturn(true);
		when(mem.read(Matchers.<byte[]>any())).thenReturn(waitForNull());
		when(mem.getAliveInterval()).thenReturn(expected);
//		when(mem.waitForWatchDogEvent()).thenReturn(true);
		doAnswer(new Answer<Boolean>()
		{

			@Override public Boolean answer(InvocationOnMock invocation)
					throws Throwable
			{
				cycles++;
				Thread.sleep(actual);
				return true;
			}
		}).when(mem).waitForWatchDogEvent();

		IServiceProtocol serivce = mock(IServiceProtocol.class);
		doAnswer(new Answer<Void>()
		{

			@Override public Void answer(InvocationOnMock invocation)
					throws Throwable
			{
				timedOut = true;
				cycles = Integer.MAX_VALUE;
				return null;
			}
		}).when(serivce).FreeInstantiate(Matchers.<Fmi2Empty>any());
		ProtocolDriver driver = new ProtocolDriver("id", serivce, mem);

		driver.open();

		while (cycles < 5)
		{
			Thread.sleep(50);
		}

		driver.close();


	}

	private byte[] waitForNull()
	{
		try
		{
			Thread.sleep(50);
		} catch (InterruptedException e)
		{
		}
		return null;
	}
}
