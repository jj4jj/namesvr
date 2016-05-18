#!/bin/bash


MYSQL="mysql -utest -p123456 "
$MYSQL -Bse "create database IF NOT EXISTS test;"
$MYSQL -Bse "create table if not exists test.name (
    type SMALLINT UNSIGNED  NOT NULL,
    name VARCHAR(192) NOT NULL,
    id BIGINT UNSIGNED NOT NULL,
    time INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY(name)
) ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci;"

