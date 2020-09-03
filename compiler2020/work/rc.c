// register control

#include<stdbool.h>

// regsiter check flag
// 0 : empty
// 1 : used
int register_check[4] = {0,0,0,0};

int count = 0;

int getRegister(){
    count++;
    if(register_check[count % 4] == 0){
        register_check[count % 4] = 1;
        return count % 4;
    }
    count++;
    if(register_check[count % 4] == 0){
        register_check[count % 4] = 1;
        return count % 4;
    }
    count++;
    if(register_check[count % 4] == 0){
        register_check[count % 4] = 1;
        return count % 4;
    }
    count++;
    if(register_check[count % 4] == 0){
        register_check[count % 4] = 1;
        return count % 4;
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