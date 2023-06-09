#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include "tags.hpp"
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

// Units of time in seconds
#define MINUTE 60
#define HOUR (60 * 60)
#define DAY (60 * 60 * 24)
#define WEEK (DAY * 7)
#define YEAR ((int)(DAY * 365.25))
#define MONTH (YEAR / 12)

struct task
{
    string name;
    string description;

    unsigned int importance;

    time_t created;
    time_t due;
};

class __pq_sort
{
public:
    bool operator()(const task &a, const task &b)
    {
        return a.due > b.due;
    }
};

ofstream &operator<<(ofstream &Stream, const task &What);
istream &operator>>(istream &Stream, task &What);
void colorPrint(const task &What);
string timeFromNow(const time_t &Then, const time_t &Now);
time_t parseTime(const string &T, const time_t &Now);

void printFromMilitaryTime(const char *What);

#endif
