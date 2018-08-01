package io.foxtrot;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.foxtrot.exptserveGrpc.exptserveStub;
import io.foxtrot.exptserveGrpc.exptserveBlockingStub;


public class FoxtrotClient {
    private final ManagedChannel channel;
    private final exptserveStub asyncStub;
    private final exptserveBlockingStub blockingStub;
    
    public FoxtrotClient(String host, int port) {
        this(ManagedChannelBuilder.forAddress(host,port).usePlaintext().build());
    }
    
    FoxtrotClient(ManagedChannel channel) {
        this.channel = channel;
        blockingStub = exptserveGrpc.newBlockingStub(channel);
        asyncStub = exptserveGrpc.newStub(channel);
    }
    
}
