#include "RestAPI.h"
#include "Output.h"
#include "Traffic.h"
#include "main.h"
#include <json/json.h>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/ThreadPool.h>
#include <Poco/Logger.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/URI.h>
#include <json/writer.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <iostream>
#include <thread>

namespace RestAPI{


void RequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
  std::string contentType { "text/plain" }; // set default content-type
  std::string output { "" };  // set null output
  // Check if the request method is GET
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
    Poco::Net::HTTPResponse::HTTPStatus status = Poco::Net::HTTPResponse::HTTP_OK;
    // Parse the URI
    Poco::URI uri(request.getURI());
    // Extract the request path
    std::string path = uri.getPath();

    if(path.find("/events") == 0) {
      std::string msg = "Request received at: '" + uri.toString() + '\'';
      Output::logger.log(Output::LogLevel::INFO, "REST API", msg);

      // Parse the queries
      std::vector<std::pair<std::string, std::string>> queryParams = uri.getQueryParameters();
      
      // Serialize the data based on the params
      auto jsonData = Traffic::serializeEventsToJSON(queryParams);
      if(jsonData) {
        // Create the writer
        Json::StreamWriterBuilder writer;
        
        // Set up minification opts
        writer["indentation"] = "";
        writer["commentStyle"] = "None";
        writer["precision"] = 6;
        writer["emitUTF8"] = true;

        // Write the data string
        output = Json::writeString(writer, *jsonData);
        // Set the content type
        contentType = "application/json";
      } else {
        status = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
        output = "Invalid query parameters.";
      }
    } else {
      status = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
      output = "Invalid request path";
      std::string errMsg = "Request received at: '" + uri.toString() + "' (\"" + output + "\")";
      Output::logger.log(Output::LogLevel::WARN, "REST API", errMsg);
    }
    response.setStatus(status);
    response.setContentType(contentType);
    std::ostream& ostr = response.send();
    ostr << output;
  } else {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
    output = "Method not allowed!";
    std::string errMsg = "Invalid request to: '" + request.getURI() + "' (\"" + output + "\")";
    Output::logger.log(Output::LogLevel::WARN, "REST API", errMsg);
    response.setContentType(contentType);
    std::ostream& ostr = response.send();
    ostr << output;
  }
}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
  (void)request;
  return new RequestHandler;
}

void startApiServer() {
  int serverPort{6969};
  Poco::Net::ServerSocket socket(serverPort);
  Poco::Net::HTTPServer server(new RequestHandlerFactory, socket, new Poco::Net::HTTPServerParams);
  server.start();
  std::string msg = "Starting REST API server on port " + std::to_string(serverPort);
  Output::logger.log(Output::LogLevel::INFO, "REST API", msg);
  std::cout << "API server running on port " << serverPort << "...\n";
  while (!programEnd) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

//int ServerApp::main(const std::vector<std::string>& args) {
//  (void)args;
//  // Create a socket for the server to listen on
//  Poco::Net::ServerSocket socket(6969);
//  // Create a server bound to a new request handler and our socket
//  Poco::Net::HTTPServer server(new RequestHandlerFactory, socket, new Poco::Net::HTTPServerParams);
//  // Start the server
//  server.start();
//  std::string msg = "Starting REST API server on port 6969"; 
//  Output::logger.log(Output::LogLevel::INFO, "REST API", msg);
//  std::cout << "API server running on port 6969...\n";
//  waitForTerminationRequest();
//  return Application::EXIT_OK;    
//}

// Check for a query parameter by key
std::optional<std::string> findQueryParam(const std::vector<std::pair<std::string, std::string>>& queryParams, const std::string& param) {
  // Search for and retrieve an iterator to our matching parameter
  // We only want the first element that matches, subsequent entries will be ignored
  auto it = std::find_if(queryParams.begin(), queryParams.end(), [&param](const auto& pair){ return pair.first == param; });
  if(it != queryParams.end()) {
    return it->second;
  }
  return std::nullopt;
}

}// namespace RestAPI
