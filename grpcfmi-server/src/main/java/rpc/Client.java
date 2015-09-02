package rpc;

import io.grpc.ChannelImpl;
import io.grpc.netty.NegotiationType;
import io.grpc.netty.NettyChannelBuilder;

import com.lausdahl.examples.FmuGrpc;
import com.lausdahl.examples.FmuGrpc.FmuBlockingStub;
import com.lausdahl.examples.Service.GetRealReply;
import com.lausdahl.examples.Service.GetRequest;
import com.lausdahl.examples.Service.SetRealRequest;
import com.lausdahl.examples.Service.SetRealRequest.Builder;
import com.lausdahl.examples.Service.VersionRequest;

public class Client {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		ChannelImpl channel = NettyChannelBuilder.forAddress("localhost", 8980)
				.negotiationType(NegotiationType.PLAINTEXT).build();

		FmuBlockingStub blockingStub = FmuGrpc.newBlockingStub(channel);

		System.out.println("Version from server is: "
				+ blockingStub.getVersion(
						VersionRequest.newBuilder().setVersion(100).build())
						.getVersion());
		
		System.out.println("Calling set reals");
		
		Builder builder = SetRealRequest.newBuilder();
		builder.addValueReference(1);
		builder.addValues(1.1);
		
		builder.addValueReference(2);
		builder.addValues(2.2);
		
		blockingStub.fmi2SetReal(builder.build());
		
		GetRealReply result = blockingStub.fmi2GetReal(GetRequest.newBuilder().addValueReference(1).addValueReference(2).build());
		
		System.out.println("Get reals - got");
		for (Double val : result.getValuesList()) {
			System.out.println("Value: "+val);
		}
	}

}
