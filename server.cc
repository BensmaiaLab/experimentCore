
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "cppServer/proto/helloWorld.pb.h"


class helloRpcImpl final : public helloRPC::Service {

}


void RunServer(const std::string& db_path) {
    std::string server_address("0.0.0.0:50051");
    helloRpcImpl service(db_path);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}


int main(int argc, char** argv) {
    // Expect only arg: --db_path=path/to/route_guide_db.json.
    std::string db = routeguide::GetDbFileContent(argc, argv);
    RunServer(db);

    return 0;
}