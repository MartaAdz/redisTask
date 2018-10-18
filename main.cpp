#include <iostream>
#include <string>
#include <hiredis/hiredis.h>

void newUser(){
    std::cout<<"User name: ";
    std::string name;
    std::cin>> name;
    auto accountNumber=(rand()%999) +100;
    void *setUser = redisCommand (c, "HMSET user %s accountNumber", name);

    std::cout<<"Credit? y/n";
    char ifCredit;
    std::cin>> ifCredit;
    if (ifCredit=='y'){
        std::cout<<"set credit limit";
        std::string creditLimit;
        std::cin>>creditLimit;
        void *setAccountLimit = redisCommand (c, "SET accountNumber:credit %s", creditLimit);
        ifCredit="credit";
    }
    else {
        std::cout<<"you have 20$ in your account";
        void *setAccountLimit = redisCommand (c, "SET accountNumber:debit 20");
        ifCredit="debit";
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

    newUser();
    

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