#include <iostream>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include "grpcpp/grpcpp.h"

#include "chat.grpc.pb.h"

ABSL_FLAG(std::string, host, "localhost:50051", "host:port");
ABSL_FLAG(std::string, username, "user", "user name");

class ChatClient {
public:
    ChatClient(std::shared_ptr<grpc::Channel> channel, std::string username)
        : stub_(Chat::NewStub(channel)), username_(username) {}

    void MakeRequests(std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>> stream) {
        std::string msg;
        
        while(true) {
            std::getline(std::cin, msg);

            if (msg == "quit") {
                break;
            }

            ChatMessage message;
            message.set_username(username_);
            message.set_msg(msg);

            stream->Write(message);
        }

        stream->WritesDone();
    }

    void ProcessResponses(std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>> stream) {
        ChatMessage message;

        while (stream->Read(&message)) {
            std::cout << absl::StrFormat("%s: %s\n", message.username(), message.msg());
        }
    }

    void StartChat() {
        grpc::ClientContext context;
        std::shared_ptr<grpc::ClientReaderWriter<ChatMessage, ChatMessage>> stream(stub_->StartChat(&context));
        std::thread writer(&ChatClient::MakeRequests, this, stream);

        ProcessResponses(stream);
        writer.join();

        grpc::Status status = stream->Finish();
        if (status.ok()) {
            std::cout << "Chat closed successfully" << std::endl;
        }else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
    }

private:
    std::shared_ptr<Chat::Stub> stub_;
    std::string username_;
};

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);

    std::string host = absl::GetFlag(FLAGS_host);
    std::string username = absl::GetFlag(FLAGS_username);

    ChatClient client(grpc::CreateChannel(host, grpc::InsecureChannelCredentials()), username);
    client.StartChat();

    return 0;

}