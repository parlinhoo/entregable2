#ifndef USER_DATA
#define USER_DATA

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
};

#endif
