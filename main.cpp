#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <hiredis/hiredis.h>

std::string createAccount(redisContext *c, std::string ID){

    int accGenerator = (rand() % 9999) + 1000;
    std::string accountNumber = std::to_string (accGenerator);

    void *newAccount = redisCommand(c, "SET %s:account %s", ID.c_str(), accountNumber.c_str());

    return accountNumber;
};
std::string getAccountNumber(redisContext *c, std::string &ID){

    redisReply *getAccountNumber = (redisReply *) redisCommand(c, "GET %s:account", ID.c_str());
    return getAccountNumber->str;
}

std::string getBalance (redisContext *c, std::string &ID, std::string accountNumber){

    redisReply *getFunds = (redisReply *) redisCommand(c, "GET  %s:account:%s",
                                                              ID.c_str(),accountNumber.c_str());
    return getFunds->str;
}

std::string getType (redisContext *c, std::string &ID, std::string accountNumber){

    redisReply *getType = (redisReply *) redisCommand(c, "GET  %s:account:%s:type",
                                                              ID.c_str(),accountNumber.c_str());
    return getType->str;
}
std::string IDgenerator(){
    using namespace std::chrono;
    std::random_device rseed;
    std::mt19937 gen(rseed()*1000*static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().
            time_since_epoch().count()));
    std::uniform_int_distribution<int> dst(100000,999999);

    int ID_Generator = dst(gen);
    std::string ID = std::to_string(ID_Generator);
    return ID;
}
std::string setUsername(){
    std::cout << "User name: ";
    std::string name;
    std::cin >> name;

    return name;
}
std::string setPassword(){
    std::cout << "Password: ";
    std::string password;
    std::cin >> password;

    return password;
}
std::string setID(){
    std::cout << "ID: ";
    std::string ID;
    std::cin >> ID;

    return ID;
}
int setFunds(){
    std::cout << "Set your funds: ";
    int funds;
    std::cin >> funds;

    return funds;
}
std::string setRecipientID() {
    std::cout << "Recipient ID: ";
    std::string recipientID;
    std::cin >> recipientID;

    return recipientID;
}


int main() {

    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }
void *flsh=redisCommand(c, "FLUSHDB");
    std::cout << "1-new user\n2-login\n0-Quit";
    int option1;
    std::cin >> option1;
    while (option1 != 0) {
        if (option1 == 1) {

            //generating an ID
            std::string ID=IDgenerator();

            void *setUser = redisCommand(c, "HMSET user:%s password %s username %s",
                    ID.c_str(), setPassword().c_str(), setUsername().c_str());

            //generating an account for a new user
            createAccount(c, ID);

            //checking if user has been created
            if (setUser != NULL) {
                std::cout << "User has been created. \nID " << ID << std::endl;
                freeReplyObject(setUser);
            } else {
                std::cout << "User has NOT been created" << c->err << std::endl;
            }

            std::cout << "next option: ";
            std::cin >> option1;

        } else if (option1 == 2) {

            std::string ID =setID();

            redisReply *getUser = (redisReply *) redisCommand(c, "HGET user:%s password", ID.c_str());

            if (getUser != NULL && getUser->str==setPassword()) {
                std::cout << "Logged in " << std::endl;

                int option2;
                std::cout << "1-Check balance \n2-Open a new credit account \n3-Open a new debit account"
                             "\n4-Pay a bill \n6-Transfer money\n0-Quit\nEnter your option: ";
                std::cin >> option2;
                while (option2 != 0) {

                    if (option2 == 1) {
                        std::string accountNumber = getAccountNumber(c, ID);
                        if (!getAccountNumber(c, ID).empty()) {
                            std::cerr << "Account could not be accessed" << std::endl;
                        } else if (!getBalance(c, ID, accountNumber).empty()) {
                            std::cout << "Account funds: " << getBalance(c, ID, accountNumber) << std::endl;
                        } else {
                            std::cerr << "Funds could not be accessed" << std::endl;
                        }

                        std::cout << "next option: ";
                        std::cin >> option2;

                    } else if (option2 == 2) {

                        //create a new account
                        std::string accountNumber = createAccount(c, ID);

                        //setting type to credit
                        void *setAccountType = redisCommand(c, "SET %s:account:%s:type credit", ID.c_str(),
                                                            accountNumber.c_str());

                        void *setAccountFunds = redisCommand(c, "SET %s:account:%s %i", ID.c_str(),
                                                             accountNumber.c_str(), setFunds());


                        std::cout << "next option: ";
                        std::cin >> option2;


                    } else if (option2 == 3) {

                        //setting type to credit
                        void *setAccountType = redisCommand(c, "SET %s:account:%s:type debit", ID.c_str(),
                                                            createAccount(c, ID).c_str());

                        void *setAccountFunds = redisCommand(c, "SET %s:account:%s %i", ID.c_str(),
                                                             getAccountNumber(c, ID).c_str(), setFunds());


                        std::cout << "next option: ";
                        std::cin >> option2;

                    } else if (option2 == 4) {

                        void *watch = redisCommand(c, "WATCH %s:account:%s", ID.c_str(), getAccountNumber(c, ID).c_str());

                        std::cout << "Amount to pay: ";
                        int amount;
                        std::cin >> amount;


                        if (getType(c, ID, getAccountNumber(c, ID)) == "debit" && amount > std::stoi(getBalance(c, ID, getAccountNumber(c, ID)))) {
                            std::cout << "Insuficient funds" << std::endl;
                        } else {

                            void *multi = redisCommand(c, "MULTI");

                            amount *= (-1);
                            void *payment = redisCommand(c, "INCRBY %s:account:%s %i", ID.c_str(), getAccountNumber(c, ID).c_str(), amount);
                        }

                        void *exitTransaction = redisCommand(c, "EXEC");

                        std::cout << "Balance: " << getBalance(c, ID, getAccountNumber(c, ID))<< std::endl;

                        std::cout << "next option: ";
                        std::cin >> option2;

                    } else if (option2 == 5) {

                        std::string recipientID = setRecipientID();
                        redisReply *getRecipient = (redisReply *) redisCommand(c, "GET %s:account ", recipientID.c_str());

                        if(getRecipient=NULL){
                            std::cout<<"No such user found in database"<<std::endl;
                        }else{

                            std::string recipientAccount = getAccountNumber(c, recipientID);
                            std::cout << "Amount to transfer: ";
                            int amount;
                            std::cin >> amount;
                            amount *= (-1);

                                        void *watch = redisCommand(c, "WATCH %s:account:%s, %s:account:%s", getAccountNumber(c, ID).c_str());
                                        void *multi = redisCommand(c, "MULTI");
                                        void *payment = redisCommand(c, "INCRBY user:account:multiple:%s:funds %i", getAccountNumber(c, ID).c_str(), amount);

                                        amount *= (-1);

                                        void *paymentReceive = redisCommand(c, "INCRBY user:account:multiple:%s:funds %i",
                                                                            recipientID.c_str(), amount);

                            void *exitOperation = redisCommand(c, "EXEC");

                            std::cout << "next option: ";
                            std::cin >> option2;
                        }

                    }
                }

            freeReplyObject(getUser);
        } else {
                std::cout << "No such user" << c->err << std::endl;
            }
            std::cout << "next option: ";
            std::cin >> option1;

        } else if (option1 == 0) std::cout << "bye Felicia";

    }


    return 0;
}