#include "../redismodule/6.2/redismodule.h"
#include "../rmutil/util.h"
#include "../rmutil/strings.h"
#include "../rmutil/test_util.h"
#include "redissolace.h"


int SolaceConfigureCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

int SolaceConnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

int SolaceDisconnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

int SolaceSendCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

int SolaceSubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

//Main method to load module
int RedisModule_OnLoad(RedisModuleCtx *ctx) {

  // Register the module itself
  if (RedisModule_Init(ctx, REDIS_SOLACE_MODULE_NAME, REDIS_SOLACE_MODULE_VERSION, REDISMODULE_APIVER_1) ==
      REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }

  // Solace configuration  
  RMUtil_RegisterReadCmd(ctx, SOLACE_CONFIGURE_COMMAND, SolaceConfigureCommand);
  // Solace connect 
  RMUtil_RegisterReadCmd(ctx, SOLACE_CONNECT_COMMAND, SolaceConnectCommand);
  // Solace disconnect
  RMUtil_RegisterReadCmd(ctx, SOLACE_DISCONNECT_COMMAND, SolaceDisconnectCommand);
  // Solace send
  RMUtil_RegisterReadCmd(ctx, SOLACE_SEND_COMMAND, SolaceSendCommand);
  // Solace subscribe
  RMUtil_RegisterWriteCmd(ctx, SOLACE_SUBSCRIBE_COMMAND, SolaceSubscribeCommand);

  return REDISMODULE_OK;
}
