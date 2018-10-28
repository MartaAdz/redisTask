#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>
#include <random>
#include <hiredis/hiredis.h>


std::string getAccountNumber(redisContext *c, std::string &ID){

    redisReply *getAccountNumber = (redisReply *) redisCommand(c, "GET %s:account", ID.c_str());
    return getAccountNumber->str;
}

std::string getBalance (redisContext *c, std::string &ID, std::string& accountNumber){

    redisReply *getFunds = (redisReply *) redisCommand(c, "GET  %s:account:%s",
                                                       ID.c_str(),accountNumber.c_str());
    return getFunds->str;
}

std::string getType (redisContext *c, std::string &ID, std::string& accountNumber){

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
std::string setRecAccount() {
    std::cout << "Recipient account: ";
    std::string recipientacc;
    std::cin >> recipientacc;

    return recipientacc;
}
int setAmount() {
    std::cout << "Amount to pay: ";
    int amount;
    std::cin >> amount;

    return amount;
}
void createUser(redisContext *c){

    std::string ID=IDgenerator();

    void *setUser = redisCommand(c, "HMSET user:%s password %s username %s",
                                 ID.c_str(), setPassword().c_str(), setUsername().c_str());

    //checking if user has been created
    if (setUser != NULL) {
        std::cout << "User has been created. \nID " << ID << std::endl;
        freeReplyObject(setUser);
    } else {
        std::cout << "User has NOT been created" << c->err << std::endl;
    }

}
std::string createAccount(redisContext *c, std::string& ID, std::string& type){

    using namespace std::chrono;
    std::random_device rseed;
    std::mt19937 gen(rseed()*1000*static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().
            time_since_epoch().count()));
    std::uniform_int_distribution<int> dst(100000,999999);

    std::string accountNumber = std::to_string (dst(gen));

    void *newAccount = redisCommand(c, "SET %s:account %s", ID.c_str(), accountNumber.c_str());
    void *setAccountType = redisCommand(c, "SET %s:account:%s:type %s", ID.c_str(),
                                        accountNumber.c_str(), type.c_str());
    void *setAccountFunds = redisCommand(c, "SET %s:account:%s %i", ID.c_str(),
                                         accountNumber.c_str(), setFunds());
    return accountNumber;
}
void transaction (redisContext *c, std::string& ID, std::string& accountNumber, std::string &recipientID, std::string &recipientAccount, int amount){
    void *multi = redisCommand(c, "MULTI");

    void *payment = redisCommand(c, "DECRBY %s:account:%s %i", ID.c_str(),
                                 accountNumber.c_str(), amount);

    void *receive = redisCommand(c, "INCRBY %s:account:%s %i", recipientID.c_str(),
                                 recipientAccount.c_str(), amount);

    void *exitTransaction = redisCommand(c, "EXEC");
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

<<<<<<< HEAD
    std::cout>>"create new user";
    //creating new user name
    std::cout<<"User name: ";
    std::string name;
    std::cin>> name;
    void *setUser = redisCommand (c, "HMSET user username %s account nill", name);

    //it's an account. it can be whether credit or debit
    std::cout<<"Credit? y/n";
    char ifCredit;
    std::cin>> ifCredit;
    int accountNumber=(rand()%999) +100;
    if (ifCredit=='y'){
        std::cout<<"Set your credit limit";
        std::string creditLimit;
        std::cin>>creditLimit;
        void *setAccountType = redisCommand (c, "HMSET user:account accountType credit");
        void *setAccountLimit = redisCommand (c, "SET user:account:accNumber:%s  %s", accountNumber, creditLimit);
    }
    else {
        std::cout<<"how much do you have in your account?";
        int funds;
        std::cin>>funds;
        void *setAccountType = redisCommand (c, "HMSET user:account accountType credit");
        void *setAccountLimit = redisCommand (c, "SET user:account:accountNumber:%s  %s", accountNumber, funds);
    }

    //one more account
    void *accountList= redisCommand (c, "HSET user:account:accountNumber multiple");
    void *accountListTransfer= redisCommand (c, "LPUSH multiple %s", accountNumber);
    int accountNumber2=(rand()%999) +100;
    if (accountNumber!=accountNumber2) {
        void *newAccount = redisCommand(c, "LPUSH multiple %s", accountNumber2);
    }
    else int accountNumber2=(rand()%999) +100;

    std::cout<<"choose account";

    std::cout<<"1-payment /n2-transfer funds to different account";
    unsigned int operation;
    std::cin>>operation;
    if (operation==1) {
        std::cout<<"Amount to pay: ";
        std::string amount;
        std::cin>>amount;
        void *watch= redisCommand (c, "WATCH user:account:accountNumber:%s", accountNumber);
        void *multi= redisCommand (c, "MULTI");
        void *payment = redisCommand (c, "INCRBY user:account:accountNumber:%s %s", accountNumber, amount);
        void *exitOperation = redisCommand (c, "EXEC");
    }
    if(operation==2) {
        std::cout<<"Amount to transfer: ";
        std::string transfer;
        std::cin>>transfer;
        void *watch= redisCommand (c, "WATCH user:account:accountNumber:%s user:account:accountNumber:%s", accountNumber);
        void *multi= redisCommand (c, "MULTI");
        void *paymentSend = redisCommand (c, "INCRBY user:account:accountNumber:%s %s", accountNumber, transfer*(-1));
        void *paymentReceive = redisCommand (c, "INCRBY user:account:accountNumber:%s %s", accountNumber2, transfer);
        void *exitOperation = redisCommand (c, "EXEC");
    }



    if (setUser != NULL) {
        std::cout<<"user is in database"<<std::endl;
        freeReplyObject(setUser);
    }
    else {
        std::cout<<"User is NOT in database"<<c->err <<std::endl;
    }
    
=======
    std::cout << "1-new user\n2-login\n0-Quit";
    int option1;
    std::cin >> option1;
    while (option1 != 0) {
        if (option1 == 1) {
>>>>>>> develop

          createUser(c);
          std::cout << "next option: ";
           std::cin >> option1;

        }
        else if (option1 == 2) {

            std::string ID =setID();

            redisReply *getUser = (redisReply *) redisCommand(c, "HGET user:%s password", ID.c_str());

            if (getUser != NULL && getUser->str==setPassword()) {
                std::cout << "Logged in " << std::endl;

                int option2;
                std::cout << "1-Check balance \n2-Open a new credit account \n3-Open a new debit account"
                             "\n4-Transfer money\n0-Quit\nEnter your option: ";
                std::cin >> option2;
                while (option2 != 0) {

                    if (option2 == 1) {
                        std::string accountNumber;
                        std::cout<<"account number: ";
                        std::cin>>accountNumber;

                        if (getAccountNumber(c, ID)==accountNumber && accountNumber.empty()) {
                            std::cerr << "Account could not be accessed" << std::endl;
                        } else if (getAccountNumber(c, ID)==accountNumber && !getBalance(c, ID, accountNumber).empty()) {
                            std::cout << "Account funds: " << getBalance(c, ID, accountNumber) << std::endl;
                        } else {
                            std::cerr << "Funds could not be accessed" << std::endl;
                        }

                        std::cout << "next option: ";
                        std::cin >> option2;

                    } else if (option2 == 2) {

                        std::string type = "credit";
                        createAccount(c, ID, type);

                        std::cout<<"Account number "<<getAccountNumber(c, ID)<<std::endl;
                        std::cout << "next option: ";
                        std::cin >> option2;


                    } else if (option2 == 3) {
                        std::string type = "debit";
                        createAccount(c, ID, type);

                        std::cout<<"Account number "<<getAccountNumber(c, ID)<<std::endl;
                        std::cout << "next option: ";
                        std::cin >> option2;


                    } else if (option2 == 4) {

                        std::string recipientID = setRecipientID();
                        std::string recipientAccount = setRecAccount();

                        redisReply *getRecipient = (redisReply *) redisCommand(c, "GET %s:account ",
                                                                               recipientID.c_str());
                        if (getRecipient->type==REDIS_REPLY_NIL || getRecipient->str != recipientAccount){
                            std::cerr<<"No such user"<<std::endl;
                        }
                        else {

			                std::string accountNumber = getAccountNumber(c, ID);
                            void *watch = redisCommand(c, "WATCH %s:account:%s", ID.c_str(),
                                                       accountNumber.c_str());

                        int amount = setAmount();


			            if (getType(c, ID, accountNumber) == "debit" &&
                                amount > std::stoi(getBalance(c, ID, accountNumber))) {
                                std::cout << "Insuficient funds" << std::endl;
                        } else {

                           transaction(c, ID, accountNumber, recipientID, recipientAccount, amount);
                        }

                        std::cout << "next option: ";
                        std::cin >> option2;
                        }
                    }
                }
            }
            else {
                std::cout << "No such user" << c->err << std::endl;
            }
            std::cout << "next option: ";
            std::cin >> option1;

        }

    else if (option1 == 0) std::cout << "bye Felicia";

    }

    return 0;
}
