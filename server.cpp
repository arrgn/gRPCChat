#include <iostream>
#include <map>
#include <mutex>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "absl/random/random.h"

#include "grpcpp/grpcpp.h"

#include "chat.grpc.pb.h"

ABSL_FLAG(uint16_t, port, 50051, "port to listen on");

class ChatServiceImpl final : public Chat::Service {
public:
    grpc::Status StartChat(grpc::ServerContext* context, grpc::ServerReaderWriter<ChatMessage, ChatMessage>* stream) override {
        uint32_t id = generareRandomId();
        {
            std::lock_guard<std::mutex> lock(mutex_);

            while (streams_.find(id) != streams_.end()) {
                id = generareRandomId();
            }
            
            streams_[id] = stream;
        }

        ChatMessage request;

        while (stream->Read(&request)) {
            broadcast(request);
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            streams_.erase(id);
        }

        return grpc::Status::OK;
    }

    void broadcast(const ChatMessage& message) {
        for (auto& stream : streams_) {
            stream.second->Write(message);
        }
    }

    inline uint32_t generareRandomId() {
        return absl::Uniform<uint32_t>(absl::BitGen());
    }

private:
    std::map<uint32_t, grpc::ServerReaderWriter<ChatMessage, ChatMessage>*> streams_;
    absl::BitGen bitgen_;
    std::mutex mutex_;
};

void RunServer(uint64_t port) {
    std::string server_address = absl::StrFormat("localhost:%d", port);
    ChatServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    RunServer(absl::GetFlag(FLAGS_port));

    return 0;
}
