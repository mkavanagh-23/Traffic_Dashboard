#ifndef RESTAPI_H
#define RESTAPI_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <vector>
#include <string>

namespace RestAPI{

class RequestHandler : public Poco::Net::HTTPRequestHandler {
public:
  // Handle a request to the server
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
  // Create a request handler
  Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;
};

class ServerApp : public Poco::Util::ServerApplication {
protected:
  int main(const std::vector<std::string>& args) override;
};

}

#endif
