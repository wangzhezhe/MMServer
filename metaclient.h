#ifndef metaclient_h
#define metaclient_h

#include "metaserver.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using metaserver::Meta;

using metaserver::PutReply;
using metaserver::PutRequest;

using metaserver::GetReply;
using metaserver::GetRequest;

using metaserver::TimeReply;
using metaserver::TimeRequest;


void recordKey(string key);


#endif