# ac-control

### Migrations

#### add preference.acTurnOffInsteadOfStop

```sqlite
alter table preference
    add acTurnOffInsteadOfStop integer default 0 not null;
```
