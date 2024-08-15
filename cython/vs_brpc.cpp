#include <butil/logging.h>
#include <brpc/server.h>

// 定义一个简单的 HTTP 服务
class MyHttpServiceImpl : public brpc::HttpServiceBase {
public:
    void HandleHttpRequest(brpc::HttpServiceContext* ctx, brpc::HttpResponse* resp) override {
        // 处理 HTTP 请求
        resp->set_content("Hello, World!");
        resp->set_content_type("text/plain");
        resp->set_status_code(brpc::HTTP_OK);
    }
};

int main(int argc, char* argv[]) {
    brpc::Server server;
    MyHttpServiceImpl my_service;

    // 添加 HTTP 服务
    if (server.AddService(&my_service, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add HTTP service";
        return -1;
    }

    // 配置服务器参数
    brpc::ServerOptions options;
    if (server.Start(8080, &options) != 0) {
        LOG(ERROR) << "Fail to start server";
        return -1;
    }

    // 运行服务器直到进程结束
    server.RunUntilAskedToQuit();
    return 0;
}