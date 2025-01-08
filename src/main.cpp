#include "vlcpp/vlc.hpp"
#include <thread>
#include <iostream>

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
