package io.foxtrot;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;


public class FoxtrotClient {
    private final ManagedChannel channel;
    
    
    public FoxtrotClient(String host, int port) {
        this(ManagedChannelBuilder.forAddress(host,port).usePlaintext().build());
    }
    
    FoxtrotClient(ManagedChannel channel) {
        this.channel = channel;
    }
    
}
