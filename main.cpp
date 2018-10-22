#include <iostream>
#include <string>
#include <vector>
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

std::string getBalance (redisContext *c, std::string &ID, std::string &accountNumber){

    redisReply *getFunds = (redisReply *) redisCommand(c, "GET  %s:account:%s",
                                                              ID.c_str(),accountNumber.c_str());
    return getFunds->str;
}

std::string getType (redisContext *c, std::string &ID, std::string &accountNumber){

    redisReply *getType = (redisReply *) redisCommand(c, "GET  %s:account:%s:type",
                                                              ID.c_str(),accountNumber.c_str());
    return getType->str;
}


void menu (std::string &ID, redisContext *c){
    int option2;
    std::cout<<"1-Check balance \n2-Open a new credit account \n3-Open a new debit account"
    "\n4-Pay a bill \n6-Transfer money\n0-Quit\nEnter your option: ";
    std::cin>>option2;
    while (option2!=0){

    if (option2==1){
        std::string accountNumber = getAccountNumber(c, ID);
        if (!getAccountNumber(c, ID).empty()) {
            std::cerr << "Account could not be accessed" << std::endl;
        }
        else if (!getBalance(c, ID, accountNumber).empty()) {
            std::cout << "Account funds: " << getBalance(c, ID, accountNumber)<< std::endl;
        } else {
            std::cerr << "Funds could not be accessed" << std::endl;
        }

        std::cout<<"next option: ";
        std::cin>>option2;

    } else if (option2 ==2) {

    //create a new account
    std::string accountNumber = createAccount(c, ID);

    //setting type to credit
    void *setAccountType = redisCommand(c, "SET %s:account:%s:type credit", ID.c_str(),
                                        accountNumber.c_str());

    std::cout << "Set your funds: ";
    int funds;
    std::cin >> funds;

    void *setAccountFunds = redisCommand(c, "SET %s:account:%s %i", ID.c_str(),
                                         accountNumber.c_str(), funds);
    //getting the balance
    std::cout << "Account funds have been set to " << getBalance(c, ID, accountNumber)<< std::endl;

    std::cout<<"next option: ";
    std::cin>>option2;


} else if(option2==3) {

    //create a new account
    std::string accountNumber = createAccount(c, ID);

    //setting type to credit
    void *setAccountType = redisCommand(c, "SET %s:account:%s:type debit", ID.c_str(),
        accountNumber.c_str());

    std::cout << "Set your funds: ";
    int funds;
    std::cin >> funds;

    void *setAccountFunds = redisCommand(c, "SET %s:account:%s %i", ID.c_str(),
                                             accountNumber.c_str(), funds);

    //getting the balance
    std::cout << "Account funds have been set to " << getBalance(c, ID, accountNumber)<< std::endl;

    std::cout<<"next option: ";
    std::cin>>option2;

} else if(option2==4){


    std::cout << "Amount to pay: ";
    int amount;
    std::cin >> amount;

    amount *= (-1);

    std::string accountNumber = getAccountNumber(c, ID);

    void *watch = redisCommand(c, "WATCH %s:account:%s", ID.c_str(), accountNumber.c_str());
    void *multi = redisCommand(c, "MULTI");

    void *payment = redisCommand(c, "INCRBY %s:account:%s %i", accountNumber.c_str(), amount);

    void *exitOperation = redisCommand(c, "EXEC");}

/*
case '6':
std::cout << "Amount to transfer: ";
int amount;
std::cin >> amount;
amount *= (-1);

void *watch = redisCommand(c, "WATCH user:account:multiple:%s:funds", accountNumber.c_str());
void *multi = redisCommand(c, "MULTI");
void *payment = redisCommand(c, "INCRBY user:account:multiple:%s:funds %i", accountNumber.c_str(), amount);

amount *= (-1);

void *paymentReceive = redisCommand(c, "INCRBY user:account:multiple:%s:funds %i",
                                    accountNumber2.c_str(), amount);
void *exitOperation = redisCommand(c, "EXEC");
*/

}

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

        std::cout<<"1-new user\n2-login";
        int option1;
        std::cin>>option1;

        if(option1==1) {

            //creating new user name
            std::cout << "User name: ";
            std::string name;
            std::cin >> name;

            std::cout << "Password: ";
            std::string password;
            std::cin >> password;

            int ID_Generator = (rand() % 9999) + 1000;
            std::string ID = std::to_string(ID_Generator);

            void *setUser = redisCommand(c, "HMSET user:%s username %s password %s", ID.c_str(), name, password);

            //generating an account for a new user
            createAccount(c, ID);

             /*redisReply *getAccountNumber = (redisReply *) redisCommand(c, "LRANGE %s:account 0 0", ID.c_str());
             std::cout<<getAccountNumber->str<<std::endl;
*/


            //checking if user has been created
            if (setUser != NULL) {
                std::cout << "User has been created. ID " << ID << std::endl;
                freeReplyObject(setUser);
            } else {
                std::cout << "User has NOT been created" << c->err << std::endl;
            }

            menu(ID, c);

        }else if(option1==2){

            std::cout << "ID: ";
            std::string ID;
            std::cin >> ID;

            std::cout << "Password: ";
            std::string password;
            std::cin >> password;

            redisReply *getUser = (redisReply *) redisCommand(c, "GET user:%s password:%s", ID.c_str(), password.c_str());

            if (getUser != NULL) {
                std::cout << "Logged in " << std::endl;
                freeReplyObject(getUser);
            } else {
                std::cout << "No such user" << c->err << std::endl;
            }

        }




    return 0;
}