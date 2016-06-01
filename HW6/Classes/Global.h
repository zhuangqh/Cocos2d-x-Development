#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <vector>
using std::string;
using std::vector;
class Global
{
public:
    Global();
    ~Global();
    static string gameSessionId;
	static string gameUsername;
    static long score;

    static string toString(vector<char> *buffer);

    static string getSessionIdFromHeader(string header);

	static void saveStatus(vector<char> *, string);

	static void loadStatus();

	static string getRandomStr();
};

#endif
