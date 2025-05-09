-- join1.sql
-- Creates a full join of all events in the database, sorted by market and update date
-- This would be a good candidate for a view

SELECT
  event_categories.name AS type,
  CONCAT_WS(' ', main_roadways.name, roadway_suffix.text) AS roadway,
  travel_direction AS direction,
  CONCAT(market_regions.name, ', ', states_provinces.abbreviation) AS market,
  event_status.name as status,
  date_reported as reported,
  date_updated as updated,
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
ORDER BY market_regions.region_id, date_updated ASC;
