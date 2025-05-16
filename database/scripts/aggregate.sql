USE mfkavanagh_255;

SELECT
  market_regions.name AS region,
  COUNT(traffic_events.event_id) AS event_count
FROM market_regions
LEFT JOIN main_roadways
  ON market_regions.region_id = main_roadways.region_id
LEFT JOIN traffic_events
  ON main_roadways.roadway_id = traffic_events.roadway_id
GROUP BY market_regions.region_id
-- Filter to groups with more records than the average
HAVING COUNT(traffic_events.event_id) > (
  -- Subquery to find the average of the number of events per region
  SELECT
    AVG(region_event_count)
  FROM (
    -- Subquery to get the number of events per region
    SELECT
      COUNT(*) AS region_event_count
    FROM traffic_events te
    JOIN main_roadways mr
      ON mr.roadway_id = te.roadway_id
    GROUP BY mr.region_id
  ) AS counts -- Derived tables must be aliased!
);
