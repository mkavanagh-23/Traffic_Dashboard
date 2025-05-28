USE mfkavanagh_255;

SELECT
  traffic_events.event_id AS id,
  event_categories.name AS type,
  CONCAT_WS(' ', main_roadways.name, roadway_suffix.abbreviation) AS roadway,
  traffic_events.travel_direction,
  traffic_events.details,
  market_regions.name AS market,
  traffic_events.date_updated
FROM traffic_events 
JOIN event_categories
  ON traffic_events.category_id = event_categories.category_id
JOIN main_roadways
  ON traffic_events.roadway_id = main_roadways.roadway_id
LEFT JOIN roadway_suffix
  ON main_roadways.suffix_id = roadway_suffix.suffix_id
JOIN market_regions
  ON main_roadways.region_id = market_regions.region_id
WHERE traffic_events.date_updated = (
  SELECT 
    MAX(te.date_updated)
  FROM traffic_events te
  JOIN main_roadways mr
    ON mr.roadway_id = te.roadway_id
  WHERE mr.region_id = main_roadways.region_id
)
ORDER BY market_regions.name ASC;
