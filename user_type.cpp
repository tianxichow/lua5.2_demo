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

#define checknpc(L)  (struct npc*)(luaL_checkudata(L , 1, "user_type.npc"))

static int new_npc(lua_State *L) {
    
    int id = luaL_checkinteger(L, 1);
    const char* name = luaL_checkstring(L ,2);
    
    struct npc *n = (struct npc *)lua_newuserdata(L, sizeof(struct npc));
    
    n->id = id;
    strcpy(n->name, name);
    
    luaL_getmetatable(L, "user_type.npc");
    lua_setmetatable(L, -2);
    
    return 1;
}

static int get_npc_id(lua_State *L) {
    struct npc *n = checknpc(L);
    
    luaL_argcheck(L, n != NULL, 1, "'npc' expect");
    
    lua_pushinteger(L, n->id);
    return 1;
}

static int get_npc_name(lua_State *L) {
    struct npc *n = checknpc(L);
    
    luaL_argcheck(L, n != NULL, 1, "'npc' expect");
    
    lua_pushstring(L, n->name);
    return 1;
}

static int set_npc_id(lua_State *L) {
    struct npc *n = checknpc(L);
    int id = luaL_checkinteger(L, 2);
    
    luaL_argcheck(L, n != NULL, 1, "'npc' expect");
    luaL_argcheck(L, id > 0, 2, "id must bigger than zero");
    
    n->id = id;
    return 0;
}

static int set_npc_name(lua_State *L) {
    struct npc *n = checknpc(L);
    const char* name = luaL_checkstring(L, 2);
    
    luaL_argcheck(L, n != NULL, 1, "'npc' expect");
    luaL_argcheck(L, name != NULL, 2, "'name' expect");
    
    strcpy(n->name, name);
    return 0;
}

static int npc2string(lua_State *L) {
    struct npc *n = checknpc(L);
    
    lua_pushfstring(L, "id:%d name:%s", n->id, n->name);
    return 1;
}

static const struct luaL_Reg npclib[] = {
    {"new", new_npc},
    {NULL, NULL}
};

static const struct luaL_Reg npclib_m[] = {
    {"get_id", get_npc_id},
    {"get_name", get_npc_name},
    {"set_id", set_npc_id},
    {"set_name", set_npc_name},
    {"__tostring", npc2string},
    {NULL, NULL}
};

int luaopen_npclib(lua_State *L) {
    luaL_newmetatable(L, "user_type.npc");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, npclib_m, 0);
    luaL_newlib(L, npclib);
    return 1;
}

int main(void) {
    
    int ret = 0;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, "npclib", luaopen_npclib, 1);
    
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
    
    lua_getglobal(L, "user_type");
    ret = lua_pcall(L, 0, 0, 0);
    
    if (ret != LUA_OK) {
        printf("lua_resume1 error: %s\n", lua_tostring(L, -1)); 
        return -1;
    }
    
    lua_close(L);
    return 0;
}