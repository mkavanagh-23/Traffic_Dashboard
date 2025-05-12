USE mfkavanagh_255;

-- outer.sql
-- Validate missing data values
-- List all regions that DO NOT have any events

SELECT
  market_regions.name AS market,
  states_provinces.name AS state
FROM market_regions
JOIN states_provinces
  ON market_regions.state_id = states_provinces.state_id
LEFT OUTER JOIN main_roadways	-- NEED TO OUTER JOIN HERE TO COPY NULL VALUES
  ON market_regions.region_id = main_roadways.region_id
LEFT OUTER JOIN traffic_events
 ON main_roadways.roadway_id = traffic_events.roadway_id
WHERE event_id IS NULL;
