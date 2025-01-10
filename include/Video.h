#ifndef VIDEO_H
#define VIDEO_H

/******* TODO *******

 - Spawn each new video on its own thread?
 - Allow for creation of multiplexed stream rather than single streams?
 - More advanced playback functionality API
 - Manage player size dynamically
 - Logic to determine whether type is a stream or a file
 - Likely need to include some kind of regex for this

*/

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

    void play();
    void stop();
};

#endif
