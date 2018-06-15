drop table testTableA;

create table testTableA (
    name char(20),
    age int,
    number int unique,
    primary key (number)
);

insert into testTableA values ("A", 18, 123);

insert into testTableA values ("B", 20, 124);

select * from testTableA;

insert into testTableA values ("C", 22, 124);

delete from testTableA where age = 20;

select * from testTableA;

insert into testTableA values ("D", 20, 124);

