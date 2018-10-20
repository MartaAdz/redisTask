#include <iostream>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>

std::string createAccount(std::string &accountNumber, redisContext *c){
    int accGenerator2 = (rand() % 9999) + 1000;
    std::string accountNumber2 = std::to_string (accGenerator2);
    do {
        if (accountNumber != accountNumber2) {
            void *newAccount = redisCommand(c, "RPUSH user:account:multiple %s", accountNumber2.c_str());
        } else accountNumber2 = (rand() % 9999) + 1000;
    }while (accountNumber==accountNumber2);

    return accountNumber2;
};

std::string getAccount(redisContext *c){

    redisReply *getAccountNumber = (redisReply *) redisCommand(c, "LRANGE user:account:multiple 0 0");
    std::cout<<getAccountNumber->str<<std::endl;
    return getAccountNumber->str;

};

void chooseAccount(redisContext *c){

    //get account number in case of multiple accounts
    int accCount;
    redisReply *accountCount = (redisReply *) redisCommand(c, "LLEN user:account:multiple");
    accCount = accountCount->integer;
    std::cout<<"acc Count: "<<accCount<<std::endl;
   /* std::vector<std::string> accountNumbers;
    int a = 0, b = 0;

    for (int i = 0; i < accCount; ++i) {
        redisReply *accountNumber= (redisReply *) redisCommand(c, "LRANGE user:account:multiple %i %i", a, b);
        accountNumbers.push_back(accountNumber->str);
        a++;
        b++;
    }

    for (int j = 0; j < accountNumbers.size(); ++j) {
        std::cout<<accountNumbers[j]<<std::endl;
    }
*/

};


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

        if(option1=1) {

            //creating new user name
            std::cout << "User name: ";
            std::string name;
            std::cin >> name;
            void *setUser = redisCommand(c, "HMSET user username %s account multiple", name);

            //generating an account for a new user
            int accGenerator = (rand() % 9999) + 1000;
            std::string accountNumber = std::to_string(accGenerator);
            void *setAccount = redisCommand(c, "LPUSH user:account:multiple %s", accountNumber.c_str());

            redisReply *getAccountNumber = (redisReply *) redisCommand(c, "LRANGE user:account:multiple 0 0");
            std::string accNum (getAccountNumber->str);
            std::cout<<"actual num: "<<accGenerator<<"Num i get : "<<accNum<<std::endl;


            //checking if user has been created
            if (setUser != NULL) {
                std::cout << "User has been created" << std::endl;
                freeReplyObject(setUser);
            } else {
                std::cout << "User has NOT been created" << c->err << std::endl;
            }
        }

        else if(option1=2){
                std::cout<<"log in";


        }



        int option2;
        std::cout<<"1-Check balance \n2-Open a new credit account \n3-Open a new debit account"
                   "\n5-Pay a bill \n6-Transfer money\n0-Quit\nEnter your option: ";
        std::cin>>option2;
        while (option2!=0){

            if(option2==1){
                //choosing account
                chooseAccount(c);

                //getting account from database
                std::string accNum = getAccount(c);

                //getting the balance
                redisReply *getAccountFunds = (redisReply *) redisCommand(c, "GET  user:account:multiple:%s:funds",
                                                                          accNum.c_str());

                //printing the response
                if (getAccountFunds != NULL && getAccountFunds->type == REDIS_REPLY_STRING) {
                    std::cout << "Account funds: " << getAccountFunds->str << std::endl;
                } else {
                    std::cerr << "Funds could not be accessed" << std::endl;
                }

                std::cout<<"next option: ";
                std::cin>>option2;
            }

             else if (option2 ==2) {
                //create a new account
                std::string accNum = getAccount(c);
                std::string accountNumber = createAccount(accNum, c);

                //setting type to credit
                void *setAccountType = redisCommand(c, "HSET user:account:multiple %s:accountType credit",
                                                    accountNumber.c_str());

                std::cout << "Set your credit limit: ";
                int creditLimit;
                std::cin >> creditLimit;
                void *setAccountLimit = redisCommand(c, "SET user:account:multiple:%s:limit %i",
                                                     accountNumber.c_str(), creditLimit);
                void *setAccountFunds = redisCommand(c, "SET user:account:multiple:%s:funds 0",
                                                     accountNumber.c_str());
                 std::cout<<"next option: ";
                 std::cin>>option2;

            }

            else if(option2=3) {

                //create a new account
                std::string accNum = getAccount(c);
                std::string accountNumber = createAccount(accNum, c);

                //setting type to credit
                void *setAccountType = redisCommand(c, "HSET user:account:multiple %s:accountType debit",
                                                    accountNumber.c_str());

                void *setAccountFunds = redisCommand(c, "SET user:account:multiple:%s:funds 0",
                                                     accountNumber.c_str());
                std::cout<<"next option: ";
                std::cin>>option2;
            }

            /*


        case '5' :
            std::cout << "Amount to pay: ";
            int amount;
            std::cin >> amount;
            amount *= (-1);
            void *watch = redisCommand(c, "WATCH user:account:multiple:%s:funds", accountNumber.c_str());
            void *multi = redisCommand(c, "MULTI");
            void *payment = redisCommand(c, "INCRBY user:account:multiple:%s:funds %i", accountNumber.c_str(), amount);
            void *exitOperation = redisCommand(c, "EXEC");

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


    return 0;
}