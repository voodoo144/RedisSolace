#include "../solclient/solClient.h"
#include "../solclient/solClientMsg.h"

/* Context */
solClient_opaqueContext_pt context_p;
solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

/* Session */
solClient_opaqueSession_pt session_p;
solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;

/* Inits Solace connection, should be called only once in main module method */
solClient_returnCode_t SolaceInit();

/* Connects to solace appliance with session properties that was configured via SolaceConfigure */
solClient_returnCode_t SolaceConnect();

/* Accepts session properties array, which will be used as connection pros */
solClient_returnCode_t SolaceConfigure(char *sessionProps[]);

/* Subscribes to topic name */
solClient_returnCode_t SolaceSubscribe(char *topicName);

/* Sends mesage to topic */
solClient_returnCode_t SolaceSend(char *topicName, char* message);

/* Close subscriptuion to specific topic */
solClient_returnCode_t SolaceUnsubscribe(char *topicName);

/* Closes solace connection */
solClient_returnCode_t SolaceDisconnect();

const char *REDIS_SOLACE_MODULE_NAME = "RedisSolace";
const int REDIS_SOLACE_MODULE_VERSION = 1;

const char *SOLACE_CONFIGURE_COMMAND = "solace.configure";
int SolaceConfigureCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char *SOLACE_CONNECT_COMMAND = "solace.connect";
int SolaceConnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char *SOLACE_DISCONNECT_COMMAND = "solace.disconnect";
int SolaceDisconnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char *SOLACE_SUBSCRIBE_COMMAND = "solace.subscribe";
int SolaceSubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char *SOLACE_UNSUBSCRIBE_COMMAND = "solace.unsubscribe";
int SolaceUnsubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

const char *SOLACE_SEND_COMMAND = "solace.send";
int SolaceSendCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

