// register control
#include<stdbool.h>

// regsiter check flag
// 0 : empty
// 1 : used
int register_check[4] = {0,0,0,0};

int count = 0;

int getRegister(){
    if(register_check[0] == 0){
        register_check[0] = 1;
        return 0;
    }
    if(register_check[1] == 0){
        register_check[1] = 1;
        return 1;
    }
    if(register_check[2] == 0){
        register_check[2] = 1;
        return 2;
    }
    if(register_check[3] == 0){
        register_check[3] = 1;
        return 3;
    }

    return -1;

}

bool rewordResiter(int n){
    if(0<=n && n<=3){
        if(register_check[n] == 0){
            register_check[n] = 1;
            return true;
        }
        else{
            return false;
        }
    }else{
        return false;
    }

}

void freeRegister(int n){
    if(0<=n && n<=3){
        register_check[n] = 0;
    }
    
}