USE mfkavanagh_255;

SELECT
  event_categories.name AS type,
  CONCAT_WS(' ', main_roadways.name, roadway_suffix.abbreviation) AS roadway,
  travel_direction,
  details,
  CONCAT(market_regions.name, ', ', states_provinces.abbreviation) AS market,
  event_status.name AS status,
  date_reported,
  date_updated,
  CONCAT(latitude, ', ', longitude) AS coordinates
FROM traffic_events 
JOIN event_categories
  ON traffic_events.category_id = event_categories.category_id
JOIN main_roadways
  ON traffic_events.roadway_id = main_roadways.roadway_id
LEFT JOIN roadway_suffix
  ON main_roadways.suffix_id = roadway_suffix.suffix_id
JOIN market_regions
  ON main_roadways.region_id = market_regions.region_id
JOIN states_provinces
  ON market_regions.state_id = states_provinces.state_id
JOIN event_status
  ON traffic_events.status_id = event_status.status_id
LEFT JOIN geo_coordinates
  ON traffic_events.event_id = geo_coordinates.event_id
ORDER BY market_regions.name ASC, date_updated DESC;
