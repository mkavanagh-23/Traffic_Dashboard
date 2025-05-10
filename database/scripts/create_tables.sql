-- Select the database
USE mfkavanagh_255;

-- Drop (delete) the tables if they exist
SET FOREIGN_KEY_CHECKS=0;
  DROP TABLE IF EXISTS `countries`;
  DROP TABLE IF EXISTS `states_provinces`;
  DROP TABLE IF EXISTS `market_regions`;
  DROP TABLE IF EXISTS `roadway_suffix`;
  DROP TABLE IF EXISTS `main_roadways`;
  DROP TABLE IF EXISTS `event_categories`;
  DROP TABLE IF EXISTS `event_status`;
  DROP TABLE IF EXISTS `traffic_events`;
  DROP TABLE IF EXISTS `geo_coordinates`;
SET FOREIGN_KEY_CHECKS=1;

-- Create the tables for the database
CREATE TABLE `countries` (
  `country_id` INT PRIMARY KEY AUTO_INCREMENT,
  `name` VARCHAR(50) NOT NULL,
  `iso_code` CHAR(2) NOT NULL UNIQUE
);

CREATE TABLE `states_provinces` (
  `state_id` INT PRIMARY KEY AUTO_INCREMENT,
  `country_id` INT NOT NULL,
  `name` VARCHAR(50) NOT NULL,
  `abbreviation` VARCHAR(10) NOT NULL UNIQUE,
  CONSTRAINT `states_fk_countries`
    FOREIGN KEY (`country_id`)
    REFERENCES `countries` (`country_id`)
);

CREATE TABLE `market_regions` (
  `region_id` INT PRIMARY KEY AUTO_INCREMENT,
  `state_id` INT NOT NULL,
  `name` VARCHAR(50) NOT NULL,
  `abbreviation` VARCHAR(10) NOT NULL UNIQUE,
  CONSTRAINT `regions_fk_states`
    FOREIGN KEY (`state_id`)
    REFERENCES `states_provinces` (`state_id`)
);

CREATE TABLE `roadway_suffix` (
  `suffix_id` INT PRIMARY KEY AUTO_INCREMENT,
  `text` VARCHAR(20) NOT NULL,
  `abbreviation` VARCHAR(8) NOT NULL
);

CREATE TABLE `main_roadways` (
  `roadway_id` INT PRIMARY KEY AUTO_INCREMENT,
  `region_id` INT NOT NULL,
  `name` VARCHAR(100) NOT NULL,
  `suffix_id` INT,
  CONSTRAINT `roadways_fk_regions`
    FOREIGN KEY (`region_id`)
    REFERENCES `market_regions` (`region_id`),
  CONSTRAINT `roadways_fk_suffix`
    FOREIGN KEY (`suffix_id`)
    REFERENCES `roadway_suffix` (`suffix_id`)
);

CREATE TABLE `event_categories` (
  `category_id` INT PRIMARY KEY AUTO_INCREMENT,
  `name` VARCHAR(60) NOT NULL UNIQUE
);

CREATE TABLE `event_status` (
  `status_id` INT PRIMARY KEY AUTO_INCREMENT,
  `name` VARCHAR(20) NOT NULL UNIQUE
);

CREATE TABLE `traffic_events` (
  `event_id` VARCHAR(64) PRIMARY KEY,
  `roadway_id` INT NOT NULL,
  `travel_direction` ENUM(
    'NB', 'SB', 'EB', 'WB', 'Both', 'Outbound', 
    'Inbound', 'Outer Loop', 'Inner Loop') NULL,
  `category_id` INT NOT NULL,
  `status_id` INT NOT NULL,
  `details` TEXT NOT NULL,
  `date_reported` DATETIME(3) NOT NULL,
  `date_updated` DATETIME(3) NOT NULL,
  CONSTRAINT `events_fk_roadways`
    FOREIGN KEY (`roadway_id`)
    REFERENCES `main_roadways` (`roadway_id`),
  CONSTRAINT `events_fk_categories`
    FOREIGN KEY (`category_id`)
    REFERENCES `event_categories` (`category_id`),
  CONSTRAINT `events_fk_status`
    FOREIGN KEY (`status_id`)
    REFERENCES `event_status` (`status_id`)
);

CREATE TABLE `geo_coordinates` (
  `event_id` VARCHAR(64) PRIMARY KEY,
  `latitude` DECIMAL(6,4) NOT NULL,
  `longitude` DECIMAL(7,4) NOT NULL,
  CONSTRAINT `coordinates_fk_events`
    FOREIGN KEY (`event_id`)
    REFERENCES `traffic_events` (`event_id`)
);

-- Create the indexes
-- Do we need to do this, and if so for which tables?
