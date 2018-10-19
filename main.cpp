#include <iostream>
#include <string>
//#include <cstdlib>
#include <hiredis/hiredis.h>

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

        std::cout<<"create new user \n";
        //creating new user name
        std::cout<<"User name: ";
        std::string name;
        std::cin>> name;
        int numGenerator = (rand()%999) +100;
        std::string accountNumber=std::to_string(numGenerator);
        void *setUser = redisCommand (c, "HMSET user username %s account %i", name, accountNumber);


        //it's an account. it can be whether credit or debit
        std::cout<<"Credit? y/n";
        char ifCredit;
        std::cin>> ifCredit;

        if (ifCredit=='y'){
            std::cout<<"Set your credit limit";
            int creditLimit;
            std::cin>>creditLimit;
            void *setAccountType = redisCommand (c, "HSET user:account accountType credit");
            void *setAccountNumber = redisCommand (c, "HSET user:account accountNumber %s", accountNumber);
            void *setAccountLimit = redisCommand (c, "SET user:account:accountNumber:%s  %i", accountNumber, creditLimit);
        }
        else {
            std::cout<<"how much do you have in your account?";
            int funds;
            std::cin>>funds;
            void *setAccountType = redisCommand (c, "HSET user:account accountType debit");
            void *setAccountNumber = redisCommand (c, "HSET user:account accountNumber %s", accountNumber);
            void *setAccountLimit = redisCommand (c, "SET user:account:accountNumber:%s  %i", accountNumber, funds);
        }

        //one more account
        void *accountList= redisCommand (c, "HSET user:account:accountNumber multiple");
        void *accountListTransfer= redisCommand (c, "LPUSH multiple %s", accountNumber);
        int numGenerator2 = (rand()%999) +100;
        std::string accountNumber2=std::to_string(numGenerator);
        if (accountNumber != accountNumber2) {
            void *newAccount = redisCommand(c, "LPUSH multiple %s", accountNumber2);
        } else {
            accountNumber2 = (rand() % 999) + 100;
            void *newAccount = redisCommand(c, "LPUSH multiple %s", accountNumber2);
        }


        //std::cout<<"choose account";


        std::cout<<"1-payment \n2-transfer funds to different account";
        unsigned int operation;
        std::cin>>operation;
        if (operation==1) {
            std::cout<<"Amount to pay: ";
            int amount;
            std::cin>>amount;
            amount*=(-1);
            void *watch= redisCommand (c, "WATCH user:account:accountNumber:%s", accountNumber);
            void *multi= redisCommand (c, "MULTI");
            void *payment = redisCommand (c, "INCRBY user:account:accountNumber:%s %i", accountNumber, amount);
            void *exitOperation = redisCommand (c, "EXEC");
        }
        if(operation==2) {
            std::cout<<"Amount to transfer: ";
            int transfer;
            std::cin>>transfer;
            transfer*=(-1);
            void *watch= redisCommand (c, "WATCH user:account:accountNumber:%s "
                                          "user:account:accountNumber:%s", accountNumber, accountNumber2);
            void *multi= redisCommand (c, "MULTI");
            void *paymentSend = redisCommand (c, "INCRBY user:account:accountNumber:%s %i", accountNumber, transfer);
            transfer*=(-1);
            void *paymentReceive = redisCommand (c, "INCRBY user:account:accountNumber:%s %i", accountNumber2, transfer);
            void *exitOperation = redisCommand (c, "EXEC");
        }


        //checkFunds
        redisReply *getAccountFunds = (redisReply*)redisCommand(c, "GET user:account:accountNumber:%s", accountNumber);
        if (getAccountFunds != NULL && getAccountFunds->type == REDIS_REPLY_STRING){
            std::cout<<"message received: "<<getAccountFunds->str<<std::endl;
        }
        else {
            std::cerr <<"wrong response"<<std::endl;
        }
        redisReply *getAccount2Funds = (redisReply*)redisCommand(c, "GET user:account:accountNumber:%s", accountNumber2);
        if (getAccount2Funds != NULL && getAccount2Funds->type == REDIS_REPLY_STRING){
            std::cout<<"message received: "<<getAccount2Funds->str<<std::endl;
        }
        else {
            std::cerr <<"wrong response"<<std::endl;
        }


        if (setUser != NULL) {
        std::cout<<"user is in database"<<std::endl;
        freeReplyObject(setUser);
    }
    else {
        std::cout<<"User is NOT in database"<<c->err <<std::endl;
    }




    redisReply *getReply = (redisReply*)redisCommand(c, "get works");
    if (getReply != NULL && getReply->type == REDIS_REPLY_STRING){
        std::cout<<"message received: "<<getReply->str<<std::endl;
        }
        else {
        std::cerr <<"wrong response"<<std::endl;
    }
    if(getReply != NULL){
        freeReplyObject(getReply);
    }
    return 0;
}