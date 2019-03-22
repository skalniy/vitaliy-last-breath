CREATE EXTENSION IF NOT EXISTS CITEXT;


drop table if exists users cascade;
drop table if exists forums cascade;
drop table if exists threads cascade;
drop table if exists posts cascade;
drop table if exists votes cascade;
drop table if exists forum_users cascade;



CREATE OR REPLACE FUNCTION drop_all_indexes() RETURNS INTEGER AS $$
DECLARE
  i RECORD;
BEGIN
  FOR i IN 
    (SELECT relname FROM pg_class WHERE relkind = 'i' AND relname NOT LIKE '%_pkey%' AND relname NOT LIKE 'pg_%')
  LOOP
    EXECUTE 'DROP INDEX ' || i.relname;
  END LOOP;
RETURN 1;
END;
$$ LANGUAGE plpgsql;

SELECT drop_all_indexes();


CREATE TABLE IF NOT EXISTS products
(
    id   bigserial primary key,
    name citext not null unique,

    cost integer not null 
);

CREATE UNIQUE INDEX IF NOT EXISTS products_name_idx ON products(name);


CREATE TABLE IF NOT EXISTS items
(
    id          bigserial primary key,
    product_id  bigint references products(id),

    updated_time      TIMESTAMP WITH TIME ZONE DEFAULT current_timestamp,
    updated_location  citext default '';
);


INSERT INTO products(name, cost) VALUES ('pic12f675', '74'), ('stm8f103', '115'), ('stm32f4', '456');