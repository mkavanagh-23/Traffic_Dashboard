#include "Video.h"

#include <iostream>

// Initialize the static VLC::Instance
VLC::Instance Video::m_instance(0, nullptr);

Video::Video(const std::string& videoSource, VLC::Media::FromType fromType)
  : m_source{videoSource}, m_type{fromType}, m_media(m_instance, m_source, m_type), m_player(m_media)
{
  if (!m_media.isValid())
    std::cerr << "Error creating media object. Is it a valid source?";
  else
    std::cout << "Successfully created media object from provided source\n";
}

void Video::play() {
  if (!m_media.isValid()) {
    std::cerr << "Invalid media source." << std::endl;
    return;
  }
  m_player.play();  // Start playback
}

void Video::stop() {
  m_player.stop();
}
