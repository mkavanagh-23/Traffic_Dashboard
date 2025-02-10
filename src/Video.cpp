#include "Video.h"
#include "Output.h"

#include <iostream>

// Initialize the static VLC::Instance
VLC::Instance Video::m_instance(0, nullptr);

Video::Video(const std::string& videoSource, VLC::Media::FromType fromType)
  : m_source{videoSource}, m_type{fromType}, m_media(m_instance, m_source, m_type), m_player(m_media)
{
  if (!m_media.isValid())
    std::cerr << Output::Colors::RED << "[VLC] Error creating media object. Is it a valid source?" << Output::Colors::END << '\n';
  else
    std::cout << Output::Colors::GREEN << "[VLC] Successfully created media object from provided source." << Output::Colors::END << '\n';
}

void Video::play() {
  if (!m_media.isValid()) {
    std::cerr << Output::Colors::RED << "[VLC] Invalid media source." << Output::Colors::END << std::endl;
    return;
  }
  m_player.play();  // Start playback
}

void Video::stop() {
  m_player.stop();
}
