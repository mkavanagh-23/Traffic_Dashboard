#include "RestAPI.h"

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/ThreadPool.h>
#include <Poco/Logger.h>
#include <Poco/Util/ServerApplication.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <iostream>

namespace RestAPI{

void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
  // Check if the request method is GET
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
      response.setContentType("text/plain");
      std::ostream& ostr = response.send();
      ostr << "Hello, world!";
  } else {
      response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
      response.setContentType("text/plain");
      std::ostream& ostr = response.send();
      ostr << "Method Not Allowed!";
  }
}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
  (void)request;
  return new RequestHandler;
}

int ServerApp::main(const std::vector<std::string>& args) {
  (void)args;
  // Create a socket for the server to listen on
  Poco::Net::ServerSocket socket(6969);
  // Create a server bound to a new request handler and our socket
  Poco::Net::HTTPServer server(new RequestHandlerFactory, socket, new Poco::Net::HTTPServerParams);
  // Start the server
  server.start();
  std::cout << "API server running on port 6969...\n";
  waitForTerminationRequest();
  return Application::EXIT_OK;    
}
}
