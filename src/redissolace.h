const char* REDIS_SOLACE_MODULE_NAME="RedisSolace";
const int REDIS_SOLACE_MODULE_VERSION=1;

const char* SOLACE_CONFIGURE_COMMAND="solace.configure";
int SolaceConfigureCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char* SOLACE_CONNECT_COMMAND="solace.connect";
int SolaceConnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char* SOLACE_DISCONNECT_COMMAND="solace.disconnect";
int SolaceDisconnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char* SOLACE_SUBSCRIBE_COMMAND="solace.subscribe";
int SolaceSubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char* SOLACE_SEND_COMMAND="solace.send";
int SolaceSendCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);


