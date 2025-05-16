USE mfkavanagh_255;

SELECT DISTINCT
    CONCAT(market_regions.name, ', ', states_provinces.abbreviation) AS market
FROM traffic_events
JOIN main_roadways
    ON traffic_events.roadway_id = main_roadways.roadway_id
JOIN market_regions
    ON main_roadways.region_id = market_regions.region_id
JOIN states_provinces
	ON market_regions.state_id = states_provinces.state_id
JOIN event_status
    ON traffic_events.status_id = event_status.status_id
WHERE NOT (
    event_status.name = 'Cleared'
  OR
    event_status.name = 'Archived'
);