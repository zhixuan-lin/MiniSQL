# MiniSQL

A mini Database Management System with full functionality in C++.

## Compile

Run

```
cmake CMakeLists.txt
make
```

for compilation. The executable `MiniSQL` will be generated under the project root.

## Test

MiniSQL is interactive. Some example executions:

Create a table:

```
create table testTableA (
    name char(20),
    age int,
    number int unique,
    primary key (number)
);
```

Select table entries:

```
select * from testTableA where age = 20;
```

Insert entries:

```
insert into testTableA values ("A", 18, 123);
```

Delete entries:

```apple js
delete from testTableA where age = 20;
```


