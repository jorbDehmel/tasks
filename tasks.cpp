/*
Jordan "Jorb" Dehmel
github.com/jorbDehmel
Protected by GPLv3
2023 - present
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
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
    // cout << "ASLDKFJL\n";

    // Name
    string name, description;

    Stream >> name;
    if (name.empty())
    {
        if (Stream.eof())
        {
            return Stream;
        }
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
        return Stream;
    }

    Stream >> What.created;
    if (Stream.eof())
    {
        return Stream;
    }

    Stream >> What.due;
    if (Stream.eof())
    {
        return Stream;
    }

    return Stream;
}

// For printing to cout
void colorPrint(const task &What)
{
    switch (What.importance)
    {
    case 0: // Most important
        cout << tags::red_bold;
        break;
    case 1:
        cout << tags::yellow_bold;
        break;
    case 2:
        cout << tags::green_bold;
        break;
    case 3:
        cout << tags::green;
        break;
    case 4: // Least important
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

int main(const int argc, const char *argv[])
{
    // Params to be loaded
    string profile = "default";
    vector<task> tasks;

    path folder = string(getenv("HOME")) + "/.tasks";

    // Create if first init
    if (!exists(folder))
    {
        // Create folder and populate
        create_directories(folder);

        ofstream settings(folder.string() + "/settings.txt");
        if (!settings.is_open())
        {
            settings.close();
            cout << tags::red_bold
                 << "Could not create settings file.\n"
                 << tags::reset;
            return 1;
        }
        settings.close();

        ofstream profile(folder.string() + "/default.tp");
        if (!profile.is_open())
        {
            profile.close();
            cout << tags::red_bold
                 << "Could not create default profile.\n"
                 << tags::reset;
            return 2;
        }
        profile.close();
    }

    // Load settings
    ifstream settings(folder.string() + "/settings.txt");
    if (!settings.is_open())
    {
        cout << tags::red_bold
             << "Could not open settings file.\n"
             << tags::reset;
        return 3;
    }

    string argName, restOfLine;
    while (!settings.eof())
    {
        settings >> argName;
        getline(settings, restOfLine);

        if (argName == "profile")
        {
            profile = restOfLine;
        }
        else if (argName != "")
        {
            cout << tags::red_bold
                 << "Unrecognized setting '" << argName << "' ignored.\n"
                 << tags::reset;
        }
    }
    settings.close();

    // Load profile
    path profilePath = folder.string() + "/" + profile + ".tp";

    ifstream prof(profilePath.string());

    task temp;
    while (!prof.eof())
    {
        prof >> temp;

        tasks.push_back(temp);

        while (prof.peek() == '\n')
        {
            prof.ignore(1);
        }
    }

    prof.close();

    // Parse args
    time_t now = time(NULL);
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "add") == 0)
        {
            if (i + 4 >= argc)
            {
                // Invalid number of args
                cout << tags::red_bold
                     << "'add' reuires four arguments: name, description, importance, and due time string.\n"
                     << tags::reset;
                return 4;
            }

            task toAdd;
            toAdd.name = argv[i + 1];
            toAdd.description = argv[i + 2];
            toAdd.importance = stoi(argv[i + 3]);
            toAdd.created = now;

            toAdd.due = parseTime(string(argv[i + 4]), toAdd.created);

            tasks.push_back(toAdd);

            i += 4;
        }
        else if (strcmp(argv[i], "done") == 0)
        {
            if (i + 1 >= argc)
            {
                // Invalid number of args
                cout << tags::red_bold
                     << "'done' requires one argument: name to remove.\n"
                     << tags::reset;
                return 4;
            }

            bool beenErased = false;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].name == argv[i + 1])
                {
                    beenErased = true;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            if (!beenErased)
            {
                cout << tags::yellow_bold
                     << "Warning: No items were erased.\n"
                     << tags::reset;
            }

            i++;
        }
        else if (strcmp(argv[i], "list") == 0)
        {
            cout << "It is currently " << ctime(&now) << '\n';

            // Quick and dirty heap sort
            priority_queue<task, vector<task>, __pq_sort> taskPQ;
            for (auto t : tasks)
            {
                taskPQ.push(t);
            }

            while (!taskPQ.empty())
            {
                cout << "Due " << ctime(&taskPQ.top().due);

                cout << timeFromNow(taskPQ.top().due, now) << '\n';

                colorPrint(taskPQ.top());
                cout << '\n';

                taskPQ.pop();
            }
        }
        else if (strcmp(argv[i], "profile") == 0)
        {
            if (i + 1 >= argc)
            {
                cout << tags::red_bold
                     << "'profile' takes one argument: profile name.\n"
                     << tags::reset;
                return 4;
            }

            cout << "Switched profile from '" << profile << "' to '";

            profile = argv[i + 1];

            cout << profile << "'\n";

            i++;
        }
        else if (strcmp(argv[i], "clear") == 0)
        {
            cout << "Clearing all overdue tasks.\n";

            bool beenErased = false;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due <= now)
                {
                    beenErased = true;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            if (!beenErased)
            {
                cout << tags::yellow_bold
                     << "Warning: No items were erased.\n"
                     << tags::reset;
            }

            cout << "Done.\n";
        }
        else if (strcmp(argv[i], "show") == 0)
        {
            if (i + 1 >= argc)
            {
                cout << tags::red_bold
                     << "'show' takes one argument: name.\n"
                     << tags::reset;
                return 4;
            }

            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].name == argv[i + 1])
                {
                    cout << "Due: " << ctime(&tasks[j].due)
                         << "(" << timeFromNow(tasks[j].due, now) << ")\n";

                    colorPrint(tasks[j]);

                    cout << "Description: " << tasks[j].description << '\n'
                         << "Created: " << ctime(&tasks[j].created) << "\n";
                }
            }

            i += 1;
        }
        else if (strcmp(argv[i], "help") == 0)
        {
            cout << tags::violet_bold
                 << "                Tasks                \n"
                 << "      Jordan Dehmel, 2023, GPL3      \n"
                 << "------------- Commands: -------------\n"
                 << " add 'NAME' 'DESCRIPTION' IMPORTANCE \n"
                 << " show 'NAME'                         \n"
                 << " done 'NAME'                         \n"
                 << " profile 'PROFILENAME'               \n"
                 << " clear                               \n"
                 << " list                                \n"
                 << " help                                \n"
                 << "-------------------------------------\n"
                 << "Importance must be an integer from 1 \n"
                 << "to 4. Enclose strings in quotes.     \n"
                 << tags::reset;
        }
        else
        {
            cout << tags::yellow_bold
                 << "Unrecognized argument '" << argv[i]
                 << "'\n"
                 << tags::reset;
        }
    }

    // Save to file
    ofstream saveProf(profilePath.string());
    if (!saveProf.is_open())
    {
        cout << tags::red_bold
             << "Could not open profile file to save changes.\n"
             << tags::reset;
        return 5;
    }

    for (task t : tasks)
    {
        saveProf << t << '\n';
    }
    saveProf.close();

    return 0;
}
