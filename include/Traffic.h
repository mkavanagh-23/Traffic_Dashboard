#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <string>
#include <string_view>
#include <vector>

namespace Traffic {
  struct Date {
    short day;
    short month;
    short year;
    short hours;
    short minutes;
    short seconds;

    Date(std::string_view dateStr);
    static int svtoi(std::string_view sv);
  };

  struct Coordinates {
    double latitude;
    double longitutde;
  };

  struct Event {
    std::string m_ID;                 // Unique ID
    std::string m_RegionName;         // Region where the event occurred
    std::string m_CountyName;         // County where the event occurred
    std::string m_Severity;           // { Unknown }
    std::string m_RoadwayName;        // Road on which event occurred
    std::string m_DirectionOfTravel;  // { None, All Directions, Northbound, Eastbound, Southbound, Westbound, Inbound, Outbound, Both Directions }
    std::string m_description;        // Often includes type, date, and affected lanes
    std::string m_Location;           // Where on the roadway the event occurred
    std::string m_LanesAffected;      // Which lanes are affected (e.g. all lanes)
    std::string m_LanesStatus;        // Status of affected lanes (e.g. closed)
    std::string m_PrimaryLocation;
    std::string m_SecondaryLocation;
    std::string m_EventType;          // { accidentsAndIncidents, roadwork, specialEvents, closures, transitMode, generalInfo, winterDrivingIndex }
    std::string m_EventSubType;
    Date m_LastUpdated;
    Coordinates m_Coordinates;
    Date m_PlannedEndDate;
    Date m_ReportedDate;
    Date m_StartDate;
  };

  struct ScheduledEvent : public Event {
    int m_ScheduleID;
    Date m_Start;
    Date m_End;
    bool m_Continuous;
    std::vector<std::string> m_ActiveDays;
    std::string m_Impact;
  };
  
  namespace NYSDOT {
  
  }

  namespace MCNY {

  }
}

#endif
