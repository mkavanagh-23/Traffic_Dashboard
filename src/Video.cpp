#include "Video.h"

#include <iostream>

// Initialize the static VLC::Instance
VLC::Instance Video::m_instance(0, nullptr);

Video::Video(const std::string& videoSource, VLC::Media::FromType fromType)
  : source{videoSource}, type{fromType}, m_media(m_instance, source, type), m_player(m_media)
{}

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