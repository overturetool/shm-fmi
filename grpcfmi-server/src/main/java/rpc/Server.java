package rpc;

import io.grpc.ServerImpl;
import io.grpc.netty.NettyServerBuilder;
import io.grpc.stub.StreamObserver;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

import com.lausdahl.examples.FmuGrpc;
import com.lausdahl.examples.Service.DoStepRequest;
import com.lausdahl.examples.Service.Empty;
import com.lausdahl.examples.Service.Fmi2StatusReply;
import com.lausdahl.examples.Service.GetBooleanReply;
import com.lausdahl.examples.Service.GetIntegerReply;
import com.lausdahl.examples.Service.GetMaxStepSizeReply;
import com.lausdahl.examples.Service.GetRealReply;
import com.lausdahl.examples.Service.GetRequest;
import com.lausdahl.examples.Service.GetStringReply;
import com.lausdahl.examples.Service.GetStringReply.Builder;
import com.lausdahl.examples.Service.InstantiateRequest;
import com.lausdahl.examples.Service.SetBooleanRequest;
import com.lausdahl.examples.Service.SetDebugLoggingRequest;
import com.lausdahl.examples.Service.SetIntegerRequest;
import com.lausdahl.examples.Service.SetRealRequest;
import com.lausdahl.examples.Service.SetStringRequest;
import com.lausdahl.examples.Service.SetupExperimentRequest;

public class Server {

	private static final Logger logger = Logger.getLogger(Server.class
			.getName());

	private final int port;

	private ServerImpl grpcServer;

	public Server(int port) {
		this.port = port;
	}

	/** Start serving requests. */
	public void start() throws IOException {
		grpcServer = NettyServerBuilder.forPort(port)
				.addService(FmuGrpc.bindService(new RouteGuideService()))
				.build().start();
		logger.info("Server started, listening on " + port);
		Runtime.getRuntime().addShutdownHook(new Thread() {
			@Override
			public void run() {
				// Use stderr here since the logger may has been reset by its
				// JVM shutdown hook.
				System.err
						.println("*** shutting down gRPC server since JVM is shutting down");
				Server.this.stop();
				System.err.println("*** server shut down");
			}
		});
	}

	/** Stop serving requests and shutdown resources. */
	public void stop() {
		if (grpcServer != null) {
			grpcServer.shutdown();
		}
	}

	public static void main(String[] args) throws Exception {
		Server server = new Server(8980);

		try {
			server.start();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

	/**
	 * Our implementation of RouteGuide service.
	 *
	 * <p>
	 * See route_guide.proto for details of the methods.
	 */
	private static class RouteGuideService implements FmuGrpc.Fmu {

		RouteGuideService() {

			reals.put(10, 9.9);
			integers.put(11, 1);
			booleans.put(12, true);
			strings.put(13, "undefined");
		}


		@Override
		public void fmi2Instantiate(InstantiateRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			// TODO Auto-generated method stub
			System.out.println("Called fmi2Instantiate");
			status(responseObserver);
		}

		@Override
		public void fmi2EnterInitializationMode(Empty request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2EnterInitializationMode");
			status(responseObserver);
		}

		private void status(StreamObserver<Fmi2StatusReply> responseObserver) {
			responseObserver.onValue(Fmi2StatusReply.newBuilder().build());
			responseObserver.onCompleted();
		}

		@Override
		public void fmi2ExitInitializationMode(Empty request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2ExitInitializationMode");
			// TODO Auto-generated method stub
			status(responseObserver);
		}

		@Override
		public void fmi2Terminate(Empty request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2Terminate");
			// TODO Auto-generated method stub
			status(responseObserver);
		}

		@Override
		public void fmi2Reset(Empty request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2Reset");
			// TODO Auto-generated method stub
			status(responseObserver);
		}

		@Override
		public void fmi2SetDebugLogging(SetDebugLoggingRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2SetDebugLogging");
			// TODO Auto-generated method stub
			status(responseObserver);
		}

		@Override
		public void fmi2GetReal(GetRequest request,
				StreamObserver<GetRealReply> responseObserver) {
			System.out.println("Called fmi2GetReal");

			com.lausdahl.examples.Service.GetRealReply.Builder builder = GetRealReply
					.newBuilder();

			for (Integer id : request.getValueReferenceList()) {
				if (reals.containsKey(id))
					builder.addValues(reals.get(id));
			}

			responseObserver.onValue(builder.build());
			responseObserver.onCompleted();
		}

		@Override
		public void fmi2GetInteger(GetRequest request,
				StreamObserver<GetIntegerReply> responseObserver) {
			System.out.println("Called fmi2GetInteger");

			com.lausdahl.examples.Service.GetIntegerReply.Builder builder = GetIntegerReply
					.newBuilder();

			for (Integer id : request.getValueReferenceList()) {
				if (integers.containsKey(id))
					builder.addValues(integers.get(id));
			}

			responseObserver.onValue(builder.build());
			responseObserver.onCompleted();
		}

		@Override
		public void fmi2GetBoolean(GetRequest request,
				StreamObserver<GetBooleanReply> responseObserver) {
			System.out.println("Called fmi2GetBoolean");

			com.lausdahl.examples.Service.GetBooleanReply.Builder builder = GetBooleanReply
					.newBuilder();

			for (Integer id : request.getValueReferenceList()) {
				if (booleans.containsKey(id))
					builder.addValues(booleans.get(id));
			}

			responseObserver.onValue(builder.build());
			responseObserver.onCompleted();

		}

		@Override
		public void fmi2GetString(GetRequest request,
				StreamObserver<GetStringReply> responseObserver) {
			System.out.println("Called fmi2GetString");

			Builder builder = GetStringReply.newBuilder();

			for (Integer id : request.getValueReferenceList()) {
				if (strings.containsKey(id))
					builder.addValues(strings.get(id));
			}

			responseObserver.onValue(builder.build());
			responseObserver.onCompleted();

		}

		@Override
		public void fmi2SetReal(SetRealRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2SetReal");

			for (int i = 0; i < request.getValueReferenceCount(); i++) {
				reals.put(request.getValueReference(i), request.getValues(i));
			}

			status(responseObserver);
		}

		@Override
		public void fmi2SetInteger(SetIntegerRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2SetInteger");

			for (int i = 0; i < request.getValueReferenceCount(); i++) {
				integers.put(request.getValueReference(i), request.getValues(i));
			}

			status(responseObserver);
		}

		Map<Integer, Double> reals = new HashMap<Integer, Double>();
		Map<Integer, Boolean> booleans = new HashMap<Integer, Boolean>();
		Map<Integer, Integer> integers = new HashMap<Integer, Integer>();
		Map<Integer, String> strings = new HashMap<Integer, String>();

		@Override
		public void fmi2SetBoolean(SetBooleanRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2SetBoolean");

			for (int i = 0; i < request.getValueReferenceCount(); i++) {
				booleans.put(request.getValueReference(i), request.getValues(i));
			}

			status(responseObserver);
		}

		@Override
		public void fmi2SetString(SetStringRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2SetString");

			for (int i = 0; i < request.getValueReferenceCount(); i++) {
				strings.put(request.getValueReference(i), request.getValues(i));
				System.out.println("\t" + request.getValues(i));
			}

			status(responseObserver);
		}

		@Override
		public void fmi2DoStep(DoStepRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			System.out.println("Called fmi2DoStep");
			// TODO Auto-generated method stub
			status(responseObserver);
		}

		@Override
		public void fmi2GetMaxStepSize(Empty request,
				StreamObserver<GetMaxStepSizeReply> responseObserver) {
			// TODO Auto-generated method stub
			System.out.println("Called fmi2GetMaxStepSize");
			
			GetMaxStepSizeReply reply =GetMaxStepSizeReply.newBuilder().setMaxStepSize(100).build();
			responseObserver.onValue(reply);
			responseObserver.onCompleted();
			
		}

		@Override
		public void fmi2SetupExperiment(SetupExperimentRequest request,
				StreamObserver<Fmi2StatusReply> responseObserver) {
			// TODO Auto-generated method stub
			System.out.println("Called fmi2SetupExperiment");
			status(responseObserver);
		}

	}

}
