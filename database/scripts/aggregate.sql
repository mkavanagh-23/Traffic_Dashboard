USE mfkavanagh_255;

-- aggregate.sql
-- List the regions that have more events than the average per-region number of events

SELECT
  market_regions.name AS region,
  COUNT(traffic_events.event_id) AS event_count
FROM market_regions
LEFT JOIN main_roadways
  ON market_regions.region_id = main_roadways.region_id
LEFT JOIN traffic_events
  ON main_roadways.roadway_id = traffic_events.roadway_id
GROUP BY market_regions.region_id
HAVING COUNT(traffic_events.event_id) > (
  SELECT 		-- Subquery to find the average of the number of events per region
    AVG(region_event_count)
  FROM (
    SELECT 		-- Subquery to get the number of events per region
      COUNT(*) AS region_event_count
    FROM traffic_events te
    JOIN main_roadways mr
      ON mr.roadway_id = te.roadway_id
    GROUP BY mr.region_id
  ) AS counts 	-- Derived tables must be aliased even if unused
);
