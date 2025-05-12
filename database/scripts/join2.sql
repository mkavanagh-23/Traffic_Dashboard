USE mfkavanagh_255;

-- join2.sql
-- List all roadways by region and state, with a count of events per row
-- Sorted by number of events, then market, then roadway
-- Be sure to include roadways with no events

SELECT
  CONCAT_WS(' ', main_roadways.name, roadway_suffix.abbreviation) AS roadway,
  market_regions.name AS market_region,
  states_provinces.name AS state,
  countries.iso_code AS country,
  COUNT(event_id) AS num_events
FROM main_roadways
LEFT JOIN roadway_suffix
  ON main_roadways.suffix_id = roadway_suffix.suffix_id
JOIN market_regions
  ON main_roadways.region_id = market_regions.region_id
JOIN states_provinces
  ON market_regions.state_id = states_provinces.state_id
JOIN countries
  ON states_provinces.country_id = countries.country_id
LEFT JOIN traffic_events
  ON main_roadways.roadway_id = traffic_events.roadway_id
GROUP BY main_roadways.roadway_id
ORDER BY `num_events` DESC, market_regions.name, main_roadways.name ASC;
