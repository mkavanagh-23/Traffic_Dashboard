#include "vlcpp/vlc.hpp"
#include <thread>
#include <iostream>

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
// Stream videos from an m3u8 playlist (obtained vis REST API)
// Mapping coordinates to regions and markets
// Severity coding
// Embed Goole Maps objects
// Create a dynamic graphical interface

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <file to play>" << std::endl;
    return 1;
  }

  // Create a VLC instance
  auto instance = VLC::Instance(0, nullptr);
  
  // Create a media object from the filepath
  auto media = VLC::Media(instance, argv[1], VLC::Media::FromPath);

  // Create a media player to play the object
  auto player = VLC::MediaPlayer(media);

  // Test media playback for 10 seconds
  player.play();
  std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
  player.stop();
  
  return 0;
}
