#include "Video.h"
#include <thread>
#include <iostream>
#include <chrono>

// Parse and store information from NYSDOT REST API
// Parse and store information for MCNY RSS feed
// GUI Window with a top bar that allows for picking between markets
// Left sidebar contains a selectable list of currently active incidents
// Incidents are refreshed every one minute
// Main content pane contains a "Details" view that contains expanded details included an embedded video stream (if available) and an embedded map
// Do we want to display embedded video feed, or just keyframes?
//
// Incorporate JSON and XML/RSS parsing
// Learn how to incorporate API key safely into project (perhaps sourced from ENV)?
// Source videos via API
// Mapping coordinates to regions and markets
// Severity coding
// Embed Goole Maps objects
// Create a dynamic graphical interface

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <URL to .m3u8 stream>" << std::endl;
    return 1;
  }

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromPath);

  // Play the video
  video.play();
  std::cout << "Now playing video: " << video.getSource() << '\n';
  std::cout << "Type: " << video.getTypeStr() << '\n';

  // Play for 15 seconds to test proper playback
  std::this_thread::sleep_for(std::chrono::seconds(15));

  // Stop media playback
  video.stop();

  return 0;
}
