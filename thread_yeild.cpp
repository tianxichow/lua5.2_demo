
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
#ifdef __cplusplus
}
#endif

static int req_id = 1;

static int l_send_and_recv(lua_State *L) {
    double x = luaL_checknumber(L, 1);
    //net send data 
    lua_pushinteger(L, x + 1);
    return lua_yield(L, 1);
}

static const struct luaL_Reg netiolib[] = {
    {"send_and_recv", l_send_and_recv},
    {NULL, NULL}
};

int luaopen_netiolib(lua_State *L) {
    luaL_newlib(L, netiolib);
    return 1;
};

int new_req(lua_State *global_L, lua_State **thread_L) {
    
    *thread_L = lua_newthread(global_L);
  
    lua_getglobal(*thread_L, "thread_yield");
    lua_pushinteger(*thread_L, req_id++);
    
    int ret = lua_resume(*thread_L, NULL, 1);
    
    if (ret != LUA_OK && ret !=  LUA_YIELD) {
        printf("lua_resume1 error: %s\n", lua_tostring(*thread_L, -1)); 
        return -1;
    }
    
    return 0;
};

int recv_data(lua_State *thread_L, int data) {
        
    lua_pushinteger(thread_L, data);
        
    int ret = lua_resume(thread_L, NULL, 2);
    
    if (ret != LUA_OK) {
        printf("lua_resume2 error: %s\n", lua_tostring(thread_L, -1)); 
        return -1;
    }
    
    int isnum;
    int result = lua_tonumberx(thread_L, -1, &isnum);
    
    if (!isnum) {
        printf("luaL_loadfile error: %s", lua_tostring(thread_L, -1)); 
        return -1;
    }
    
    printf("result %d\n",result);
    return 0;
}

int main(void) {
    
    int ret = 0;
    lua_State *global_L, *thread1_L, *thread2_L;
    
    global_L = luaL_newstate();
    luaL_openlibs(global_L);
    luaL_requiref(global_L, "netiolib", luaopen_netiolib, 1);
    
    ret = luaL_loadfile(global_L, "demo.lua");
    if (ret != LUA_OK) {
        printf("luaL_loadfile error: %s\n", lua_tostring(global_L, -1)); 
        return -1;
    }
    
    ret = lua_pcall(global_L, 0, 0, 0);
    if (ret != LUA_OK) {
        printf("lua_pcall error: %s\n", lua_tostring(global_L, -1)); 
        return -1;
    }

    ret = new_req(global_L, &thread1_L);
  
    if (ret != 0) {
        printf("new_req 1 error\n"); 
        return -1;
    }
    
    ret = new_req(global_L, &thread2_L);
  
    if (ret != 0) {
        printf("new_req 2 error\n"); 
        return -1;
    }
    
    ret = recv_data(thread2_L , 100);

    if (ret != 0) {
        printf("recv_data 2 error\n"); 
        return -1;
    }   
  
    ret = recv_data(thread1_L , 10);

    if (ret != 0) {
        printf("recv_data 1 error\n"); 
        return -1;
    } 
    
    lua_close(global_L);
    return 0;
}