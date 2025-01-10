#ifndef VIDEO_H
#define VIDEO_H

#include <vlcpp/vlc.hpp>
#include <string>

class Video {
  public:
    const std::string source;
    const VLC::Media::FromType type;

  private:
    // VLC Media Members
    static VLC::Instance m_instance;  // make static, we only need one instance for all videos
    VLC::Media m_media;
    VLC::MediaPlayer m_player;

  public:
    Video(const std::string& videoSource, VLC::Media::FromType fromType);
    ~Video(){}

    void play();
    void stop();
};

#endif