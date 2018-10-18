#include <iostream>
#include <string>
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

    std::cout<<"User name: ";
    std::string name;
    std::cin>> name;
    void *setUser = redisCommand (c, "HMSET user %s LT100", name);
    if (setUser != NULL) {
        std::cout<<"user is in database"<<std::endl;
        freeReplyObject(setUser);
    }
    else {
        std::cout<<"TUser is NOT in database"<<c->err <<std::endl;
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