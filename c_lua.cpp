
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

struct npc {
  int id;
  char name[128];  
};

static int l_extra(lua_State *L) {
    struct npc *n = (struct npc *)lua_topointer(L ,1);
    printf("npc id:%d, name:%s\n",n->id, n->name);
    return 0;
}

static const struct luaL_Reg myclib[] = {
    {"extra", l_extra},
    {NULL, NULL}
};

int luaopen_myclib(lua_State *L) {
    luaL_newlib(L, myclib);
    return 1;
}

static void StackDump(lua_State *L) {
    int i;
    int top = lua_gettop(L);
    printf("StackDump:start---\n");
    for (int i = 1; i <= top; ++i) {
        int t = lua_type(L, i);
        printf("%d-%s:", i, lua_typename(L, t));
        switch (t) {
            
            case LUA_TSTRING: {
                printf("%s", lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN: {
                printf("%s", lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER: {
                printf("%g", lua_tonumber(L, i));
                break;
            }
            default: {
                break;
            }           
        }  
        printf("\n");
    }
    printf("StackDump:end---\n");
}

int main(void) {
    
    int ret = 0;
    lua_State *L;
    
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, "myclib", luaopen_myclib, 1);
    
    ret = luaL_loadfile(L, "demo.lua");
    if (ret != LUA_OK) {
        printf("luaL_loadfile error: %s\n", lua_tostring(L, -1)); 
        return -1;
    }
    
    ret = lua_pcall(L, 0, 0, 0);
    if (ret != LUA_OK) {
        printf("lua_pcall error: %s\n", lua_tostring(L, -1)); 
        return -1;
    }
        
    struct npc n;
    n.id =1;
    sprintf(n.name, "rangechow");
    
    lua_getglobal(L, "c_lua");
    lua_pushlightuserdata(L, &n);
    StackDump(L);
    
    ret = lua_pcall(L, 1, 1, 0);
    
    if (ret != LUA_OK) {
        printf("lua_pcall error: %s\n", lua_tostring(L, -1)); 
        return -1;
    }
    
    int isnum;
    int result = lua_tonumberx(L, -1, &isnum);
    
    if (!isnum) {
        printf("lua_tonumberx error: %s", lua_tostring(L, -1)); 
        return -1;
    }
    
    printf("result %d\n",result);
    
    lua_close(L);
    return 0;
}