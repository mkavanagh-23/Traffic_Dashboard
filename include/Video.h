#ifndef VIDEO_H
#define VIDEO_H

#include <vlcpp/vlc.hpp>
#include <string>
#include <string_view>

class Video {
  public:

  private:
    // Video members
    const std::string m_source;
    const VLC::Media::FromType m_type;

    // VLC Media Members
    static VLC::Instance m_instance;  // make static, we only need one instance for all videos
    VLC::Media m_media;
    VLC::MediaPlayer m_player;

  public:
    Video(const std::string& videoSource, VLC::Media::FromType fromType);
    ~Video(){}

    std::string_view getSource(){ return m_source; }
    VLC::Media::FromType getType(){ return m_type; }
    std::string_view getTypeStr(){ return (m_type == VLC::Media::FromLocation) ? "Stream" : "File"; }

    void play();    // Start playback
    void stop();    // End playback
};

#endif
