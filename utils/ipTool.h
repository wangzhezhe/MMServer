


#ifndef IPTOOL_H
#define IPTOOL_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <set>

using namespace std;

string parseIP(string peerURL);

string parsePort(string peerURL);

void recordIPPortWithoutFile(string &ipstr,string INTERFACE);

void createDir(string DirPath);

void deleteDir(string DirPath);

int getFreePortNum();

#endif