USE mfkavanagh_255;

SELECT DISTINCT
  market_regions.name AS market,
  states_provinces.name AS state
FROM market_regions
JOIN states_provinces
  ON market_regions.state_id = states_provinces.state_id
LEFT OUTER JOIN main_roadways
  ON market_regions.region_id = main_roadways.region_id
LEFT OUTER JOIN traffic_events
 ON main_roadways.roadway_id = traffic_events.roadway_id
WHERE event_id IS NULL;
