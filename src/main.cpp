#include "Traffic/NYSDOT.h"
#include "Traffic/MCNY.h"
#include "Video.h"
#include "Output.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define JSON_DLL
#endif

/* TODO: 
 *  Parse and store information for MCNY RSS feed
 *  Add methods for accessing cameras from NYSDOT API
 *  GUI Window with a top bar that allows for picking between markets
 *  Left sidebar contains a selectable list of currently active incidents
 *  Incidents are refreshed every one minute
 *  Main content pane contains a "Details" view that contains expanded details
 *  include an embedded video stream (if available) and an embedded map 
 *  Do we want to display embedded video feed, or just keyframes?
 *  Incorporate XML/RSS parsing
 *  Mapping coordinates to regions and markets
 *  Severity coding
 *  Embed Goole Maps objects
 *  Create a dynamic graphical interface
 */

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << Output::Colors::RED << "usage: " << argv[0] << " <video source stream/url>" << Output::Colors::END << std::endl;
    return 1;
  }

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromLocation);
  video.play();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  video.stop();
  
  // Test Event Parsing
  if(!Traffic::NYSDOT::getEnv())
    return 1;
  if(!Traffic::NYSDOT::getEvents())
    return 1;
  if(!Traffic::MCNY::getEvents())
    return 1;

  return 0;
}
