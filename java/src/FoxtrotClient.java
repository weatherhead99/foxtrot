package io.foxtrot;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.foxtrot.exptserveGrpc.exptserveStub;
import io.foxtrot.exptserveGrpc.exptserveBlockingStub;
import io.foxtrot.Foxtrot;

import java.util.concurrent.TimeUnit;
import java.util.HashMap;

public class FoxtrotClient {
    private final ManagedChannel channel;
    private final exptserveBlockingStub blockingStub;
    
    public FoxtrotClient(String host, int port) {
        this(ManagedChannelBuilder.forAddress(host,port).usePlaintext().build());
    }
    
    FoxtrotClient(ManagedChannel channel) {
        this.channel = channel;
        blockingStub = exptserveGrpc.newBlockingStub(channel);
    }

    public Foxtrot.servdescribe DescribeServer() {
	var em = Foxtrot.empty.newBuilder().build();
	var desc = blockingStub.describeServer(em);
	return desc;
    }
}
