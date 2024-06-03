#ifndef USER_DATA
#define USER_DATA

#include <iostream>
#include <ostream>
#include <string>

struct UserData {
    std::string university;
    unsigned long long int userID;
    std::string userName;
    long long int numTweets;
    long long int friendsCount;
    long long int followersCount;
    std::string createdAt;

    UserData() : 
        university(""), 
        userID(0),
        userName(""), 
        numTweets(0), 
        friendsCount(0), 
        followersCount(0),
        createdAt("") {}

    friend std::ostream& operator<<(std::ostream &os, const UserData &data) {
        os << "(Usuario: " << data.userName << ", " << data.userID << ")";
        return os;
    }
};

#endif
