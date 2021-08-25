#include "solclient/solClient.h"
#include "solclient/solClientMsg.h"

/* Context */
solClient_opaqueContext_pt context_p;
solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

/* Session */
solClient_opaqueSession_pt session_p;
solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;


solClient_returnCode_t SolaceInit();

solClient_returnCode_t SolaceConnect();

solClient_returnCode_t SolaceConfigure(char* sessionProps[] );

solClient_returnCode_t SolaceSubscribe(char* topicName);

solClient_returnCode_t SolaceUnsubscribe(char* topicName);

solClient_returnCode_t SolaceDisconnect();