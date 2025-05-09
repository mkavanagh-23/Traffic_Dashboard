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
  (DEFAULT, 'Street', 'St'),
  (DEFAULT, 'Road', 'Rd'),
  (DEFAULT, 'Boulevard', 'Blvd'),
  (DEFAULT, 'Way', 'Wy'),
  (DEFAULT, 'Parkway', 'Pkwy'),
  (DEFAULT, 'Drive', 'Dr'),
  (DEFAULT, 'Lane', 'Ln');

INSERT INTO `main_roadways` VALUES  
  (DEFAULT, 1, 'W Genesee', 1),
  (DEFAULT, 1, 'Rt. 11', NULL),
  (DEFAULT, 1, 'Henry Clay', 3),
  (DEFAULT, 1, 'E Fayette', 1),
  (DEFAULT, 1, 'Cherry', 1),
  (DEFAULT, 1, 'Sackett', 1),
  (DEFAULT, 2, 'North', 1),
  (DEFAULT, 2, 'Calkins', 2),
  (DEFAULT, 2, 'Rt. 490', NULL)
  (DEFAULT, 2, 'N Greece', 2),
  (DEFAULT, 2, 'Fairport', 2),
  (DEFAULT, 3, 'I-90', NULL),
  (DEFAULT, 2, 'I-90', NULL),
  (DEFAULT, 7, 'Hwy 401', NULL),
  (DEFAULT, 7, 'Hwy 400', NULL)
  (DEFAULT, 7, 'Queen Elizabeth', 4),
  (DEFAULT, 7, 'Don Valley', 5),
  (DEFAULT, 9, 'Hwy 417', NULL),
  (DEFAULT, 9, 'Riverside', 6),
  (DEFAULT, 8, 'Hwy 15', NULL),
  (DEFAULT, 8, 'Metropolitan', NULL),
  (DEFAULT, 8, 'Hwy 138', NULL),
  (DEFAULT, 6, 'I-80', NULL),
  (DEFAULT, 6, 'I-287', NULL);

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
  ('ROCE2512801103', 9, 'WB', 1, 1, 'at Ames St [Exit Ramp]', '2025-05-08 14:12:00.000', '2025-05-08 14:23:22.414'),
  ('GREE2512902079', 10, NULL, 1, 1, 'at Latta Rd', '2025-05-08 14:23:03.000', '2025-05-08 14:50:01.224'),
  ('PITE2512902116', 11, 'EB', 3, 2, 'at Washington Rd', '2025-05-08 14:49:23.000', '2025-05-08 14:51:09.213'),
  ('CAD-251290228', 12, 'EB', 1, 1, 'at After I-190 [Exit 53 - Buffalo (Downtown)]', '2025-05-08 14:23:02.000', '2025-05-08 14:46:22.466'),
  ('CAD-251290233', 13, 'WB', 2, 4, 'at Before Rt. 332 [Exit 44 - Canadaigua]', '2025-05-08 09:27:01.000', '2025-05-08 14:48:03.694'),
  ('ROCE2512902210', 9, 'WB', 2, 1, 'at S Winton Rd', '2025-05-08 14:39:00.000', '2025-05-08 14:50:23.666'),
  ('PERE2512902200', 9, 'WB', 1, 5, 'at Pittsford Rd [Exit 26 Off-Ramp]', '2025-05-08 14:12:00.000', '2025-05-08 14:39:07.213'),
  ('ONT-29736', 14, 'EB', 2, 1, 'at Mavis Rd [Collector - Off-Ramp]', '2025-05-08 14:07:00.000', '2025-05-08 14:37:03.202'),
  ('ONT-29727', 15, 'NB', 8, 1, 'between Canal Rd and Mapleview Dr', '2025-05-08 14:27:00.000', '2025-05-08 14:38:09.202'),
  ('ONT-29725', 14, 'EB', 4, 1, 'at Dixie Rd [On-Ramp]', '2025-05-08 14:02:00.000', '2025-05-08 14:51:21.202')
  ('ONT-29721', 16, 'SB', 1, 2, 'at Ford Dr', '2025-05-08 13:56:23.000', '2025-05-08 14:34:29.231'),
  ('ONT-29754', 16, 'NB', 2, 1, 'at Fifty Rd', '2025-05-08 14:56:23.000', '2025-05-08 14:59:28.231'),
  ('ONT-29746', 14, 'EB', 2, 1, 'at Whites Rd', '2025-05-08 14:25:23.000', '2025-05-08 14:43:28.231'),
  ('ONT-29742', 14, 'WB', 2, 1, 'at Kipling Ave', '2025-05-08 14:39:22.000', '2025-05-08 14:51:28.223'),
  ('ONT-29759', 17, 'NB', 2, 1, 'at Lawrence Ave', '2025-05-08 14:23:25.000', '2025-05-08 14:43:29.523'),
  ('OTT-29716', 18, 'EB', 1, 1, 'at Panmure Rd', '2025-05-08 14:21:29.000', '2025-05-08 14:47:29.523')
  ('OTT-29715', 18, 'WB', 8, 1, 'at March Rd', '2025-05-07 12:22:29.000', '2025-05-08 14:50:29.523');
  ('OTT-28921', 18, 'EB', 2, 5, 'at Rochester St', '2025-05-08 12:22:27.000', '2025-05-08 14:42:29.503'),
  ('OTT-29699', 19, 'SB', 1, 5, 'at Hunt Club Rd', '2025-05-08 13:45:27.000', '2025-05-08 14:51:27.222')
  ('MTL-3487245', 20, 'SB', 1, 1, 'at Rue Jean-Talon', '2025-05-08 14:22:27.000', '2025-05-08 14:51:29.222'),
  ('MTL-3487253', 21, 'EB', 2, 1, 'at Hwy 15', '2025-05-08 14:34:29.000', '2025-05-08 14:53:55.222'),
  ('MTL-3487266', 22, 'WB', 1, 4, 'at Rue Clement', '2025-05-08 14:27:27.000', '2025-05-08 14:43:22.222'),
  ('CCT-45933295', 23, 'EB', 1, 1, 'at Parsippany Rd'),
  ('CCT-45933359', 23, 'EB', 1, 2, 'at Rt. 46 (Denville)'),
  ('CCT-45935560', 24, 'NB', 1, 5, 'at I-80 (Parsippany)');

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
  ('ROCE2512801103', 43.1572, -77.6440),
  ('GREE2512902079', 43.2536, -77.7328),
  ('PITE2512902116', 43.1092, -77.4998),
  ('CAD-251290228', 42.8769, -78.7830),
  ('CAD-251290233', 42.9953, -77.3581),
  ('ROCE2512902210', 43.1448, -77.5557),
  ('PERE2512902200', 43.0791, -77.4855),
  ('ONT-29736', 43.6192, -79.7094),
  ('ONT-29727', 44.0524, -79.6094),
  ('ONT-29725', 43.6485, -79.6378),
  ('ONT-29721', 43.4946, -79.6739),
  ('ONT-29754', 43.2172, -79.6370),
  ('ONT-29746', 43.8166, -79.1145),
  ('ONT-29742', 43.7008, -79.5617),
  ('ONT-29759', 43.7504, -79.3334),
  ('OTT-29716', 45.3327, -76.1618),
  ('OTT-29715', 45.3130, -76.0569),
  ('OTT-28921', 45.4028, -75.7109),
  ('OTT-29699', 45.3345, -75.6937),
  ('MTL-3487245', 45.4946, -73.6529),
  ('MTL-3487253', 45.5041, -73.6653),
  ('MTL-3487266', 45.4340, -73.6502);

<title>MVA / NO INJURIES at RT 441 EB/LINDEN AVE BRI</title>
<link>https://www.monroecounty.gov/safety-incidents</link>
<pubDate>Fri, 09 May 2025 15:42:00 -0400</pubDate>
<description>Status: WAITING, ID: BRIE2512902261</description>
<guid>https://www.monroecounty.gov/safety-incidents?BRIE2512902261</guid>
<geo:lat>+43.128</geo:lat>
<geo:long>-77.5217</geo:long>

<title>DANGEROUS CONDITION-IMMEDIATE THREAT at JEFFERSON RD/LOMB MEMORIAL DR BRI</title>
<link>https://www.monroecounty.gov/safety-incidents</link>
<pubDate>Fri, 09 May 2025 15:30:00 -0400</pubDate>
<description>Status: ENROUTE, ID: BRIE2512902233</description>
<guid>https://www.monroecounty.gov/safety-incidents?BRIE2512902233</guid>
<geo:lat>+43.0923</geo:lat>
<geo:long>-77.6747</geo:long>

<title>MVA / NO INJURIES at EAST AVE/GIBBS ST ROC</title>
<link>https://www.monroecounty.gov/safety-incidents</link>
<pubDate>Fri, 09 May 2025 15:27:00 -0400</pubDate>
<description>Status: ONSCENE, ID: ROCE2512902221</description>
<guid>https://www.monroecounty.gov/safety-incidents?ROCE2512902221</guid>
<geo:lat>+43.1569</geo:lat>
<geo:long>-77.6018</geo:long>

<title>MVA / NO INJURIES at WHITNEY RD E/WATSON RD PER</title>
<link>https://www.monroecounty.gov/safety-incidents</link>
<pubDate>Fri, 09 May 2025 16:17:00 -0400</pubDate>
<description>Status: ENROUTE, ID: PERE2512902383</description>
<guid>https://www.monroecounty.gov/safety-incidents?PERE2512902383</guid>
<geo:lat>+43.1083</geo:lat>
<geo:long>-77.4322</geo:long>

<title>MVA / NO INJURIES at WHITNEY RD E/WATSON RD PER</title>
<link>https://www.monroecounty.gov/safety-incidents</link>
<pubDate>Fri, 09 May 2025 16:17:00 -0400</pubDate>
<description>Status: ENROUTE, ID: PERE2512902383</description>
<guid>https://www.monroecounty.gov/safety-incidents?PERE2512902383</guid>
<geo:lat>+43.1083</geo:lat>
<geo:long>-77.4322</geo:long>

Bridge work on I-690 eastbound between I-81 Northbound Interchange (Syracuse) and Exit 14 - Teall Avenue (Syracuse), Continuous Monday August 12th, 2024 09:00 AM thru Tuesday July 1st, 2025 06:00 PM 1 Right lane of 3 lanes closed
Start Time	Aug 12 2024, 9:00 AM
Recurrence	Continuous Monday August 12, 2024 9:00 AM to Tuesday July 1, 2025 6:00 PM, Sunday thru Saturday.
Last Updated	Mar 12 2025, 6:31 PM

Longterm construction on I-81 northbound between Exit 25A - I-90 (Salina) and Exit 26 - US 11; Mattydale (Salina), Continuous Wednesday January 1st,2025 06:00 AM thru Wednesday December 31st,2025 03:00 PM 1 Right lane of 4 lanes closed northbound,
Start Time	Jan 1 2025, 6:00 AM
Recurrence	Continuous Wednesday January 1, 2025 6:00 AM to Wednesday December 31, 2025 3:00 PM, Sunday thru Saturday.
Last Updated	Apr 18 2025, 2:00 PM

Bridge work on I-81 southbound between Exit 28 - Taft Road; North Syracuse (Cicero) and Exit 27- Syracuse Airport (Salina), Continuous Thursday March 27th,2025 09:00 AM thru Friday May 16th,2025 03:30 PM 1 Left lane of 3 lanes closed southbound,
Start Time	Mar 27 2025, 9:00 AM
Recurrence	Continuous Thursday March 27, 2025 9:00 AM to Friday May 16, 2025 3:30 PM, Sunday thru Saturday.
Last Updated	May 1 2025, 3:30 PM

Construction and road rehabilitation on US 62 both directions Exit - I-290 Youngmann Expressway; Ridge Lea Road (Amherst) to Connecting Boulevard (Wheatfield), Wednesday April 9th, 2025 thru Saturday January 31st, 2026, Sunday thru Saturday, 12:00 AM thru 11:59 PM 1 Right lane of 4 lanes closed
Start Time	Apr 9 2025, 12:00 AM
Recurrence	Wednesday April 9, 2025 12:00 AM to Saturday January 31, 2026 11:59 PM, Sunday thru Saturday.
Last Updated	Apr 9 2025, 12:10 PM

Bridge work on US 20 both directions Leydecker Road (Orchard Park) to Exit - Aurora Expressway (Elma), Thursday April 24th, 2025 thru Friday August 29th, 2025, Sunday thru Saturday, 07:00 AM thru 05:00 PM 1 Right lane of 2 lanes closed
Start Time	Apr 24 2025, 7:00 AM
Recurrence	Thursday April 24, 2025 7:00 AM to Friday August 29, 2025 5:00 PM, Sunday thru Saturday.
Last Updated	Apr 24 2025, 8:42 AM

Crash on I-90 - NYS Thruway eastbound at Before Exit 25 (I-90) - Schenectady (I-890) starting 3:03 PM, 05/09/2025 [CARS CAD-251290226]
Start Time	May 9 2025, 3:03 PM
Last Updated	May 9 2025, 3:03 PM

Crash on I-87 Northway northbound before Exit 8; Crescent Road (Halfmoon) right shoulder closed
Lane Closures	right shoulder closed
Start Time	May 9 2025, 3:17 PM
Last Updated	May 9 2025, 3:17 PM

Bridge rehabilitation on I-87 Northway southbound ramp to Exit 7; NY 7 (Colonie), Continuous Sunday April 13th, 2025 12:00 AM thru Tuesday September 30th, 2025 11:59 PM 1 Left lane of 2 lanes closed
Start Time	Apr 13 2025, 12:00 AM
Recurrence	Continuous Sunday April 13, 2025 12:00 AM to Tuesday September 30, 2025 11:59 PM, Monday.
Last Updated	Apr 14 2025, 10:17 AM

Construction on NY 32 southbound at simmons lane (Menands) Lane Shift SB, Monday May 5th, 2025 thru Friday May 9th, 2025, Monday thru Friday, 07:00 AM thru 05:00 PM All lanes open
Start Time	May 5 2025, 7:00 AM
Recurrence	Monday May 5, 2025 7:00 AM to Friday May 9, 2025 5:00 PM, Monday thru Friday.
Last Updated	May 3 2025, 2:44 AM

Long Term Road Construction on I-80 Westbound At Exit 34 - NJ 15 (Wharton) Monday March 31, 2025 01:05 PM thru Wednesday June 25, 2025 12:00 PM, All lanes closed and detoured
Start Time	Mar 31 2025, 1:05 PM
Recurrence	Continuous Monday March 31, 2025 1:05 PM to Wednesday June 25, 2025 12:00 PM, Monday.
Last Updated	Apr 29 2025, 3:43 PM
