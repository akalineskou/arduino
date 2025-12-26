# ac-control

### Truncate Tables

```sqlite
DELETE
FROM commands;
DELETE
FROM logs;
DELETE
FROM temperature_readings;

DELETE
FROM SQLITE_SEQUENCE
WHERE name IN (
               'commands',
               'logs',
               'temperature_readings'
    );
```

### Migrations

#### add preference.acTurnOffInsteadOfStop

```sqlite
alter table preference
    add acTurnOffInsteadOfStop integer default 0 not null;
```

#### add logs

```sqlite
CREATE TABLE logs
(
    id       integer not null
        constraint id
            primary key autoincrement,
    log      TEXT    not null,
    filename TEXT    not null,
    line     integer not null,
    time     integer
);
```
