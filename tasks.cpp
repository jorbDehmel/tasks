/*
Jordan "Jorb" Dehmel
github.com/jorbDehmel
Protected by GPLv3
2023 - present
*/

#include "helpers.hpp"

#define VERSION "0.0.1"

int main(const int argc, const char *argv[])
{
    if (argc == 1)
    {
        cout << tags::red_bold
             << "Please enter an argument (help for more info).\n"
             << tags::reset;
        return 4;
    }

    // Params to be loaded
    vector<task> tasks;
    map<string, string> settingsMap;
    settingsMap["profile"] = "default";

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

        while (restOfLine[0] == ' ')
        {
            restOfLine = restOfLine.substr(1);
        }
        while (restOfLine.back() == ' ')
        {
            restOfLine.pop_back();
        }

        settingsMap[argName] = restOfLine;
    }
    settings.close();

    // Load profile
    path profilePath = folder.string() + "/" + settingsMap["profile"] + ".tp";

    ifstream prof(profilePath.string());

    task temp;
    while (!prof.eof())
    {
        prof >> temp;

        if (temp.name == "READ ERROR")
        {
            break;
        }

        tasks.push_back(temp);

        while (prof.peek() == '\n')
        {
            prof.ignore(1);
        }
    }

    prof.close();

    time_t now = time(NULL);
    cout << "-------------------------------------\n"
         << "It is " << tags::violet_bold << ctime(&now) << tags::reset
         << "Profile '" << settingsMap["profile"] << "' has " << tasks.size() << " tasks.\n"
         << "-------------------------------------\n\n";

    // Parse args
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "add") == 0 || strcmp(argv[i], "new") == 0)
        {
            if (i + 4 >= argc)
            {
                // Invalid number of args
                cout << tags::red_bold
                     << "'add' requires four arguments: name, description, importance, and due time string.\n"
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

            cout << "Added item '" << argv[i + 1] << "'\n\n";

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

            int numRemoved = 0;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].name == argv[i + 1])
                {
                    numRemoved++;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            cout << "Erased " << numRemoved << " items.\n\n";

            i++;
        }
        else if (strcmp(argv[i], "list") == 0)
        {
            // Quick and dirty heap sort
            priority_queue<task, vector<task>, __pq_sort> taskPQ;
            for (auto t : tasks)
            {
                taskPQ.push(t);
            }

            while (!taskPQ.empty())
            {
                cout << timeFromNow(taskPQ.top().due, now) << '\n';
                colorPrint(taskPQ.top());
                cout << "\n";

                taskPQ.pop();
            }

            if (tasks.size() == 0)
            {
                cout << "There are no items to list.\n\n";
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

            cout << "Switched profile from '" << settingsMap["profile"] << "' to '";
            settingsMap["profile"] = argv[i + 1];
            cout << settingsMap["profile"] << "'\n\n";

            i++;
        }
        else if (strcmp(argv[i], "clear") == 0)
        {
            cout << "Clearing all overdue tasks.\n";

            int numRemoved = 0;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due <= now)
                {
                    numRemoved++;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            cout << "Erased " << numRemoved << " items.\n\n";
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

            bool matched = false;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].name == argv[i + 1])
                {
                    matched = true;
                    cout << "Name:        | " << tasks[j].name << '\n'
                         << "Description: | " << tasks[j].description << '\n'
                         << "Importance:  | " << tasks[j].importance << '\n'
                         << "Created at:  | " << ctime(&tasks[j].created)
                         << "Due at:      | " << ctime(&tasks[j].due)
                         << timeFromNow(tasks[j].due, now) << "\n\n";
                }
            }

            if (!matched)
            {
                cout << "'" << argv[i + 1] << "' did not match any tasks.\n\n";
            }

            i += 1;
        }
        else if (strcmp(argv[i], "doneall") == 0)
        {
            cout << "Erased " << tasks.size() << " items.\n\n";
            tasks.clear();
        }
        else if (strcmp(argv[i], "donethru") == 0)
        {
            if (i + 1 >= argc)
            {
                cout << tags::red_bold
                     << "'donethru' takes one argument: end time.\n"
                     << tags::reset;
                return 4;
            }

            time_t thresh = parseTime(argv[i + 1], now);

            int numRemoved = 0;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due <= thresh)
                {
                    numRemoved++;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            cout << "Erased " << numRemoved << " items.\n\n";
            i += 1;
        }
        else if (strcmp(argv[i], "donebet") == 0)
        {
            if (i + 2 >= argc)
            {
                cout << tags::red_bold
                     << "'donebet' takes two arguments: start time and end time.\n"
                     << tags::reset;
                return 4;
            }

            time_t lower = parseTime(argv[i + 1], now);
            time_t upper = parseTime(argv[i + 2], now);

            int numRemoved = 0;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due >= lower && tasks[j].due < upper)
                {
                    numRemoved++;
                    tasks.erase(tasks.begin() + j);
                    j--;
                }
            }

            cout << "Erased " << numRemoved << " items.\n\n";
            i += 2;
        }
        else if (strcmp(argv[i], "showthru") == 0)
        {
            if (i + 1 >= argc)
            {
                cout << tags::red_bold
                     << "'showthru' takes one argument: end time.\n"
                     << tags::reset;
                return 4;
            }

            time_t thresh = parseTime(argv[i + 1], now);

            bool matched = false;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due < thresh)
                {
                    matched = true;
                    cout << "Name:        | " << tasks[j].name << '\n'
                         << "Description: | " << tasks[j].description << '\n'
                         << "Importance:  | " << tasks[j].importance << '\n'
                         << "Created at:  | " << ctime(&tasks[j].created)
                         << "Due at:      | " << ctime(&tasks[j].due)
                         << timeFromNow(tasks[j].due, now) << "\n\n";
                }
            }

            if (!matched)
            {
                cout << "No items fell within this time range.\n\n";
            }

            i += 1;
        }
        else if (strcmp(argv[i], "showbet") == 0)
        {
            if (i + 1 >= argc)
            {
                cout << tags::red_bold
                     << "'showbet' takes two arguments: start time and end time.\n"
                     << tags::reset;
                return 4;
            }

            time_t lower = parseTime(argv[i + 1], now);
            time_t upper = parseTime(argv[i + 2], now);

            bool matched = false;
            for (int j = 0; j < tasks.size(); j++)
            {
                if (tasks[j].due >= lower && tasks[j].due < upper)
                {
                    matched = true;
                    cout << "Name:        | " << tasks[j].name << '\n'
                         << "Description: | " << tasks[j].description << '\n'
                         << "Importance:  | " << tasks[j].importance << '\n'
                         << "Created at:  | " << ctime(&tasks[j].created)
                         << "Due at:      | " << ctime(&tasks[j].due)
                         << timeFromNow(tasks[j].due, now) << "\n\n";
                }
            }

            if (!matched)
            {
                cout << "No items fell within this time range.\n\n";
            }

            i += 2;
        }
        else if (strcmp(argv[i], "help") == 0)
        {
            cout << "                Tasks                \n"
                 << "      Jordan Dehmel, 2023, GPL3      \n"
                 << "------------- Commands: -------------\n"
                 << " add/new 'NAME' 'DESC' IMP TIMESTRING\n"
                 << " show 'NAME'                         \n"
                 << " done 'NAME'                         \n"
                 << " donethru TIMESTRING                 \n"
                 << " donebet TIMESTRING TIMESTRING       \n"
                 << " doneall                             \n"
                 << " showthru TIMESTRING                 \n"
                 << " showbet TIMESTRING TIMESTRING       \n"
                 << " profile 'PROFILENAME'               \n"
                 << " clear                               \n"
                 << " list                                \n"
                 << " help                                \n"
                 << "-------------------------------------\n"
                 << "Importance must be 1, 2 or 3.        \n"
                 << "Enclose strings in quotes.           \n"
                 << "Timestrings must take the form:      \n"
                 << "'1y2m3w4d5h6i7s', where the numbers  \n"
                 << "denote periods of time from now and  \n"
                 << "the letters denote units (y = year,  \n"
                 << "m = month, w = week, d = day, h =    \n"
                 << "hour, i = minute, s = second).       \n"
                 << "-------------------------------------\n"
                 << "Version " << VERSION << "\n\n";
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

    ofstream saveSettings(folder.string() + "/settings.txt");
    if (!saveSettings.is_open())
    {
        cout << tags::red_bold
             << "Could not open settings file to save changes.\n"
             << tags::reset;
        return 6;
    }

    for (pair<string, string> item : settingsMap)
    {
        if (item.first != "")
        {
            saveSettings << item.first << " " << item.second << '\n';
        }
    }

    saveSettings.close();

    return 0;
}
