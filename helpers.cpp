#include "helpers.hpp"

// For saving to files ONLY
ofstream &operator<<(ofstream &Stream, const task &What)
{
    Stream << '"' << What.name << "\" \"" << What.description
           << "\" " << What.importance << " " << (long int)What.created << " "
           << (long int)What.due;

    return Stream;
}

istream &operator>>(istream &Stream, task &What)
{
    if (Stream.eof())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    // Name
    string name, description;

    Stream >> name;
    if (name.empty())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    if (name.front() == '"')
    {
        while (name.back() != '"')
        {
            string temp;
            Stream >> temp;
            name += " " + temp;
        }

        name = name.substr(1, name.size() - 2);
    }
    else if (name.front() == '\'')
    {
        while (name.back() != '\'')
        {
            string temp;
            Stream >> temp;
            name += " " + temp;
        }

        name = name.substr(1, name.size() - 2);
    }

    // Description
    Stream >> description;
    if (description.empty())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    if (description.front() == '"')
    {
        while (description.back() != '"')
        {
            string temp;
            Stream >> temp;
            description += " " + temp;
        }

        description = description.substr(1, description.size() - 2);
    }
    else if (description.front() == '\'')
    {
        while (description.back() != '\'')
        {
            string temp;
            Stream >> temp;
            description += " " + temp;
        }

        description = description.substr(1, description.size() - 2);
    }

    What.name = name;
    What.description = description;

    // Rest of data
    Stream >> What.importance;
    if (Stream.eof())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    Stream >> What.created;
    if (Stream.eof())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    Stream >> What.due;
    if (Stream.eof())
    {
        What.name = What.description = "READ ERROR";
        What.created = What.due = What.importance = -1;
        return Stream;
    }

    return Stream;
}

// For printing to cout
void colorPrint(const task &What)
{
    switch (What.importance)
    {
    case 3: // Most important
        cout << tags::red_bold;
        break;
    case 2:
        cout << tags::yellow_bold;
        break;
    case 1:
        cout << tags::green_bold;
        break;
    default: // Finished or invalid
        cout << tags::strikeout;
        break;
    }

    cout << What.name << " (" << What.importance << ")\n"
         << tags::reset;

    return;
}

string timeFromNow(const time_t &Then, const time_t &Now)
{
    long int dif = abs(Then - Now);
    int years, months, weeks, days, hours, minutes, seconds;

    years = dif / YEAR;
    dif %= YEAR;

    months = dif / MONTH;
    dif %= MONTH;

    weeks = dif / WEEK;
    dif %= WEEK;

    days = dif / DAY;
    dif %= DAY;

    hours = dif / HOUR;
    dif %= HOUR;

    minutes = dif / MINUTE;
    dif %= MINUTE;

    seconds = dif;

    string out;
    if (years != 0)
    {
        out += to_string(years) + ((years == 1) ? " year " : " years ");
    }
    if (months != 0)
    {
        out += to_string(months) + ((months == 1) ? " month " : " months ");
    }
    if (weeks != 0)
    {
        out += to_string(weeks) + ((weeks == 1) ? " week " : " weeks ");
    }
    if (days != 0)
    {
        out += to_string(days) + ((days == 1) ? " day " : " days ");
    }
    if (hours != 0)
    {
        out += to_string(hours) + ((hours == 1) ? " hour " : " hours ");
    }
    if (minutes != 0)
    {
        out += to_string(minutes) + ((minutes == 1) ? " minute " : " minutes ");
    }
    if (seconds != 0)
    {
        out += to_string(seconds) + ((seconds == 1) ? " second " : " seconds ");
    }

    // Future or past
    if (Then < Now)
    {
        out += "ago";
    }
    else
    {
        out += "from now";
    }

    return out;
}

time_t parseTime(const string &T, const time_t &Now)
{
    /*
    For instance:
    1y2m3w4d5h6i7s
    */

    time_t out = Now;
    string cur;

    try
    {
        for (char c : T)
        {
            switch (c)
            {
            case 'y': // years
                out += stoi(cur) * YEAR;
                cur = "";
                break;
            case 'm': // months
                out += stoi(cur) * MONTH;
                cur = "";
                break;
            case 'w': // weeks
                out += stoi(cur) * WEEK;
                cur = "";
                break;
            case 'd': // days
                out += stoi(cur) * DAY;
                cur = "";
                break;
            case 'h': // hours
                out += stoi(cur) * HOUR;
                cur = "";
                break;
            case 'i': // minutes
                out += stoi(cur) * MINUTE;
                cur = "";
                break;
            case 's': // seconds
                out += stoi(cur);
                cur = "";
                break;
            default:
                cur += c;
                break;
            }
        }
    }
    catch (...)
    {
        cout << tags::red_bold
             << "Could not decipher string '" << T << "'\n"
             << tags::reset;
        out = 0;
    }

    return out;
}

// Because I do not like ctime's 24-hour clock
void printFromMilitaryTime(const char *What)
{
    // Ex: 'Fri Jun  9 15:12:45 2023'
    // To: 'Fri Jun  9 3:12:45 PM 2023'
    // The 4th block of non-space before any colon
    // Modulo this by 12 if it is >12
    // Add AM if <12, add PM if >= 12

    char dayOfWeek[32], month[32], day[32], timeRem[32], year[32];
    int hour = -1;

    sscanf(What, "%31s %31s %31s %d:%31s %31s",
           &dayOfWeek, &month, &day, &hour, &timeRem, &year);

    printf("%s %s %s %d:%s %s %s\n",
           dayOfWeek, month, day,
           hour > 12 ? hour % 12 : hour,
           timeRem,
           (hour > 11 ? "PM" : "AM"),
           year);

    return;
}
