drop table testTableA;

create table testTableA (
    name char(20),
    age int,
    number int unique,
    primary key (number)
);

select * from testTableA;

insert into testTableA values ("A", 18, 123);

select * from testTableA;

insert into testTableA values ("B", 20, 124);

select * from testTableA;

insert into testTableA values ("X", 18, 125);

select * from testTableA;

insert into testTableA values ("C", 22, 124);

delete from testTableA where age = 20;

select * from testTableA;

insert into testTableA values ("D", 20, 124);

select name from testTableA;

select * from testTableA where age > 16 and age < 19;

select * from testTableA;

delete from testTableA where age > 16 and number > 19;

select * from testTableA;

insert into testTableA values ("E", 20, 124);

select * from testTableA where age = 20 and number = 124;