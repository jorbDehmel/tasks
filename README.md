# Tasks
Jordan Dehmel, 2023, jdehmel@outlook.com

## Overview

Tasks is a command line planner. It keeps track of items
composed of a name, a description, a level of importance,
creation time and due time. It is ideal for assignments
and tasks.

## Installation

Tasks is only compatible with Linux systems. To install,
simply run `make install` while in this directory.

## Uninstallation

To uninstall, simply run `make uninstall` while in this
directory.

## Commands and Usage

`tasks help` will list all of these commands.

### Adding or Removing Tasks

Command        | Arguments                                 | Purpose
---------------|-------------------------------------------|--------------------------------------
`add` or `new` | name, description, importance, timestring | Adds a new task.
`done`         | name                                      | Removed some task 'name'.
`donethru`     | timestring                                | Removes any tasks before some time.
`donebet`      | start timestring, end timestring          | Removes any tasks between two times.
`doneall`      |                                           | Removes all tasks.
`clear`        |                                           | Removes any overdue tasks.

### Viewing Tasks

Command        | Arguments                        | Purpose
---------------|----------------------------------|----------------------------------------
`show`         | name                             | Shows the details of the task 'name'.
`showthru`     | timestring                       | Shows any tasks before some time.
`showbet`      | start timestring, end timestring | Shows any tasks between two times.
`list`         |                                  | Shows all tasks.

### Misc

Command        | Arguments    | Purpose
---------------|--------------|----------------------------------
`help`         |              | Shows help.
`profile`      | profile name | Switches to a different profile.

### Timestrings

Letter | Meaning
-------|--------
y      | year
m      | month
w      | week
d      | day
h      | hour
i      | minute
s      | second

A timestring is some combination of these letters and
numbers, denoting some time from the current moment.
For instance, one week from right now would be '1w'.
One year, six months, one week, 2 days, 12 hours, 10
minutes and 16 seconds would be '1y6m1w2d12h10i16s'.

### Profiles

A profile saves all tasks associated with it. Going
to a new profile will not modify your old data in any
way.
