CREATE DATABASE `fys` /*!40100 DEFAULT CHARACTER SET latin1 */;

CREATE TABLE `team_character` (
  `id_character` int(11) NOT NULL AUTO_INCREMENT,
  `id_user` char(45) NOT NULL,
  `name` varchar(45) NOT NULL,
  `life_max` int(11) NOT NULL,
  `life` int(11) NOT NULL,
  `secondary_max` int(11) DEFAULT NULL,
  `secondary` int(11) DEFAULT NULL,
  PRIMARY KEY (`id_character`,`id_user`),
  UNIQUE KEY `id_character_UNIQUE` (`id_character`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;

# Only handled by LocalContent for now (NAS) Maybe switch to database later on
CREATE TABLE `contenders` (
  `key` char(45) NOT NULL,
  `content` varchar(20000) NOT NULL,
  `name` varchar(45) NOT NULL,
  PRIMARY KEY (`key`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

# Only handled by LocalContent for now (NAS) Maybe switch to database later on
CREATE TABLE `attack` (
  `id_attack` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `content` varchar(10000) NOT NULL,
  PRIMARY KEY (`id_attack`),
  UNIQUE KEY `attack_id_UNIQUE` (`id_attack`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

# Only handled by LocalContent for now (NAS) Maybe switch to database later on
CREATE TABLE `encounters` (
  `key` char(45) NOT NULL,
  `content` varchar(5000) NOT NULL,
  PRIMARY KEY (`key`),
  UNIQUE KEY `key_UNIQUE` (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `character_attack` (
  `id_character` int(11) NOT NULL,
  `id_attack` int(11) NOT NULL,
  `key_attack` varchar(45) NOT NULL,
  PRIMARY KEY (`id_character`,`id_attack`),
  KEY `fk_character_attack_1` (`id_attack`),
  CONSTRAINT `fk_character_attack_1` FOREIGN KEY (`id_attack`) REFERENCES `attack` (`id_attack`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_character_attack_2` FOREIGN KEY (`id_character`) REFERENCES `team_character` (`id_character`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
