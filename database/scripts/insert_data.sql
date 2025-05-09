-- Select the database
USE mfkavanagh_255;

-- Insert data into the tables
INSERT INTO `countries` VALUES 
  (DEFAULT, 'United States of America', 'US'),
  (DEFAULT, 'Canada', 'CA');

INSERT INTO `states_provinces` VALUES
  (DEFAULT, 2, 'Ontario', 'ON'),
  (DEFAULT, 2, 'Quebec', 'QC'),
  (DEFAULT, 1, 'New York', 'NY'),
  (DEFAULT, 1, 'New Jersey', 'NJ');

INSERT INTO `market_regions` VALUES
  (DEFAULT, 3, 'Syracuse', 'SYR'),
  (DEFAULT, 3, 'Rochester', 'ROC'),
  (DEFAULT, 3, 'Buffalo', 'BUF'),
  (DEFAULT, 3, 'Albany', 'ALB'),
  (DEFAULT, 3, 'Binghamton', 'BNG'),
  (DEFAULT, 4, 'Sussex', 'SUS'),
  (DEFAULT, 1, 'Toronto', 'TO'),
  (DEFAULT, 2, 'Montreal', 'MTL'),
  (DEFAULT, 1, 'Ottawa', 'OTT');

INSERT INTO `roadway_suffix` VALUES
  (DEFAULT, 'St'),
  (DEFAULT, 'Rd'),
  (DEFAULT, 'Blvd');

INSERT INTO `main_roadways` VALUES  
  (DEFAULT, 1, 'W Genesee', 1),
  (DEFAULT, 1, 'Rt. 11', NULL),
  (DEFAULT, 1, 'Henry Clay', 3),
  (DEFAULT, 1, 'E Fayette', 1),
  (DEFAULT, 1, 'Cherry', 1),
  (DEFAULT, 1, 'Sackett', 1),
  (DEFAULT, 2, 'North', 1),
  (DEFAULT, 2, 'Calkins', 2),
  (DEFAULT, 2, 'Rt. 490', NULL);

INSERT INTO `event_categories` VALUES
  (DEFAULT, 'Accident'),
  (DEFAULT, 'Disabled Vehicle'),
  (DEFAULT, 'Vehicle Fire'),
  (DEFAULT, 'Police Activity'),
  (DEFAULT, 'Structure Fire'),
  (DEFAULT, 'Medical Event'),
  (DEFAULT, 'Special Event'),
  (DEFAULT, 'Construction'),
  (DEFAULT, 'Severe Weather'),
  (DEFAULT, 'Ruptured Pipeline'),
  (DEFAULT, 'Traffic Light Malfunction');

INSERT INTO `event_status` VALUES
  (DEFAULT, 'Active'),
  (DEFAULT, 'Dispatched'),
  (DEFAULT, 'Pending'),
  (DEFAULT, 'Waiting'),
  (DEFAULT, 'Cleared');

INSERT INTO `traffic_events` VALUES
  ('ONGOV-ZZVCHI0508251045MADNTSMOTIESWGEWRD', 1, NULL, 1, 1, 'at Sleepy Hollow Rd', '2025-05-08 14:45:00.000', '2025-05-08 14:48:49.338'),
  ('ONGOV-TLF0508250758NEWICEASSISTROUE11MOLSRD', 2, NULL, 6, 5, 'between Moltion Rd and Sentinel Heights Rd', '2025-05-08 11:58:00.000', '2025-05-08 14:17:00.448'),
  ('ONGOV-TSL0508251003SHEICELOSRTYHENLVDCLARDR', 3, NULL, 2, 1, 'at Clayton Manor Dr', '2025-05-08 14:03:00.000', '2025-05-08 14:29:00.578'),
  ('ONGOV-TSL0508251037NEWICELOSRTYHENLVDWETLRD', 3, 'SB', 1, 1, 'at Wetzel Rd', '2025-05-08 14:07:00.000', '2025-05-08 14:37:03.202'),
  ('ONGOV-SYR0507251458SYRICESUSOUSEFAESTWESNST', 4, NULL, 4, 1, 'between Westcott St and Allen St (Parkside Commons)', '2025-05-07 18:58:00.000', '2025-05-08 14:17:00.491'),
  ('ONGOV-SYR0508250934SYRICERUPINECHEEST', 5, NULL, 10, 1, 'at E Fayette St', '2025-05-08 13:34:00.000', '2025-05-08 14:17:00.448'),
  ('ONGOV-SYR0508250713SYRICEMOTIESSACTSTAPPAVE', 6, 'NB', 1, 3, 'between Apple St and Park Ave', '2025-05-08 11:13:00.000', '2025-05-08 14:17:00.448'),
  ('ONGOV-TCI0508251044CICICEMOTIESBREE31', 2, 'NB', 3, 5, '[Brewerton Rd] at Rt. 31', '2025-05-08 14:44:00.000', '2025-05-08 14:45:40.575'),
  ('ROCE2512800767', 7, NULL, 1, 2, 'at St Jacob St (with Entrapment)', '2025-05-08 12:23:00.000', '2025-05-08 14:17:01.181'),
  ('HENE2512800849', 8, NULL, 11, 1, 'at Pinnacle Rd (Flashing Red)', '2025-05-08 12:54:00.000', '2025-05-08 14:17:01.178'),
  ('ROCE2512801103', 9, 'WB', 1, 1, 'at Ames St [Exit Ramp]', '2025-05-08 14:12:00.000', '2025-05-08 14:23:22.414');

INSERT INTO `geo_coordinates` VALUES
  ('ONGOV-ZZVCHI0508251045MADNTSMOTIESWGEWRD', 43.0449, -75.8815),
  ('ONGOV-TLF0508250758NEWICEASSISTROUE11MOLSRD', 42.9089, -76.1136),
  ('ONGOV-TSL0508251003SHEICELOSRTYHENLVDCLARDR', 43.1159, -76.1853),
  ('ONGOV-TSL0508251037NEWICELOSRTYHENLVDWETLRD', 43.1492, -76.1881),
  ('ONGOV-SYR0507251458SYRICESUSOUSEFAESTWESNST', 43.0482, -76.1159),
  ('ONGOV-SYR0508250934SYRICERUPINECHEEST', 43.0483, -76.1235),
  ('ONGOV-SYR0508250713SYRICEMOTIESSACTSTAPPAVE', 43.0516, -76.1763),
  ('ONGOV-TCI0508251044CICICEMOTIESBREE31', 43.1755, -76.1192),
  ('ROCE2512800767', 43.1781, -77.5953),
  ('HENE2512800849', 43.0688, -77.5881),
  ('ROCE2512801103', 43.1572, -77.6440);
