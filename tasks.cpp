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
#define DAY (60 * 60 * 24)
#define WEEK (DAY * 7)
#define MONTH (DAY * 3.5)
#define YEAR (DAY * 365.25)

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
    Stream << '"' << What.name << "\", \"" << What.description
           << "\", " << What.importance << ", " << (long int)What.created << ", "
           << (long int)What.due << ",\n";

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
            throw runtime_error("Could not create settings file.");
        }
        settings.close();

        ofstream profile(folder.string() + "/default.tp");
        if (!profile.is_open())
        {
            profile.close();
            throw runtime_error("Could not create default profile.");
        }
        profile.close();
    }

    // Load settings
    ifstream settings(folder.string() + "/settings.txt");
    if (!settings.is_open())
    {
        throw runtime_error("Could not open settings file.");
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
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "add") == 0)
        {
            if (i + 3 >= argc)
            {
                // Invalid number of args
                throw runtime_error("'add' reuires three arguments: name, description, and importance.");
            }

            task toAdd;
            toAdd.name = argv[i + 1];
            toAdd.description = argv[i + 2];
            toAdd.importance = stoi(argv[i + 3]);
            toAdd.created = time(NULL);

            // Ask about due time
            // TEMP
            toAdd.due = toAdd.created + DAY;

            i += 3;
        }
        else if (strcmp(argv[i], "done") == 0)
        {
            if (i + 1 >= argc)
            {
                // Invalid number of args
                throw runtime_error("'done' requires one argument: name to remove.");
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
            time_t cur = time(NULL);
            cout << "It is currently " << ctime(&cur) << '\n';

            priority_queue<task, vector<task>, __pq_sort> taskPQ;
            for (auto t : tasks)
            {
                taskPQ.push(t);
            }

            while (!taskPQ.empty())
            {
                cout << "Due " << ctime(&taskPQ.top().due);
                colorPrint(taskPQ.top());
                cout << '\n';

                taskPQ.pop();
            }
        }
        else if (strcmp(argv[i], "profile") == 0)
        {
            if (i + 1 >= argc)
            {
                throw runtime_error("'profile' takes one argument: profile name.");
            }

            cout << "Switched profile from '" << profile << "' to '";

            profile = argv[i + 1];

            cout << profile << "'\n";

            i++;
        }
        else if (strcmp(argv[i], "help") == 0)
        {
            cout << tags::violet_bold
                 << "                Tasks                \n"
                 << "      Jordan Dehmel, 2023, GPL3      \n"
                 << "------------- Commands: -------------\n"
                 << " add 'NAME' 'DESCRIPTION' IMPORTANCE \n"
                 << " done 'NAME'                         \n"
                 << " profile 'PROFILENAME'               \n"
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

    return 0;
}
