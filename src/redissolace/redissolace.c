#include "../redismodule/6.2/redismodule.h"
#include "../rmutil/util.h"
#include "../rmutil/strings.h"
#include "../rmutil/test_util.h"
#include "redissolace.h"
#include "../solclient/solClient.h"
#include "../solclient/solClientMsg.h"

void ParseModuleArgsAsStrings(RedisModuleString **argv, int argc, char *destArray)
{
    for (size_t i = 1; i < argc; i++)
    {
        char *tmp;
        RMUtil_ParseArgs(argv, argc, i, "c", &tmp);
        destArray[i - 1] = tmp;
    }
}

/*****************************************************************************
 * sessionMessageReceiveCallback
 *
 * The message callback is invoked for each Direct message received by
 * the Session. In this sample, the message is printed to the screen.
 *****************************************************************************/
solClient_rxMsgCallback_returnCode_t sessionMessageReceiveCallback(solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p)
{
    char buffer[5000];
    solClient_msg_dump(msg_p, buffer, 5000);

    solClient_int64_t rxSeqNum;
    const char     *senderId_p;

    if ( solClient_msg_getSequenceNumber ( msg_p, &rxSeqNum ) != SOLCLIENT_OK ) {
            rxSeqNum = 0;
    }

    if ( solClient_msg_getSenderId ( msg_p, &senderId_p ) != SOLCLIENT_OK ) {
            senderId_p = "";
    }

    return SOLCLIENT_CALLBACK_OK;
}

/*****************************************************************************
 * sessionEventCallback
 *
 * The event callback function is mandatory for session creation.
 *****************************************************************************/
void sessionEventCallback(solClient_opaqueSession_pt opaqueSession_p,
                          solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p)
{
    solClient_errorInfo_pt errorInfo_p;

    switch (eventInfo_p->sessionEvent)
    {
    case SOLCLIENT_SESSION_EVENT_UP_NOTICE:
    case SOLCLIENT_SESSION_EVENT_ACKNOWLEDGEMENT:
    case SOLCLIENT_SESSION_EVENT_TE_UNSUBSCRIBE_OK:
    case SOLCLIENT_SESSION_EVENT_CAN_SEND:
    case SOLCLIENT_SESSION_EVENT_RECONNECTING_NOTICE:
    case SOLCLIENT_SESSION_EVENT_RECONNECTED_NOTICE:
    case SOLCLIENT_SESSION_EVENT_PROVISION_OK:
    case SOLCLIENT_SESSION_EVENT_SUBSCRIPTION_OK:
    case SOLCLIENT_SESSION_EVENT_DOWN_ERROR:
    case SOLCLIENT_SESSION_EVENT_CONNECT_FAILED_ERROR:
    case SOLCLIENT_SESSION_EVENT_REJECTED_MSG_ERROR:
    case SOLCLIENT_SESSION_EVENT_SUBSCRIPTION_ERROR:
    case SOLCLIENT_SESSION_EVENT_RX_MSG_TOO_BIG_ERROR:
    case SOLCLIENT_SESSION_EVENT_TE_UNSUBSCRIBE_ERROR:
    case SOLCLIENT_SESSION_EVENT_PROVISION_ERROR:
    default:
        break;
    }
}

solClient_returnCode_t SolaceInit()
{
    if (solClient_initialize(SOLCLIENT_LOG_DEFAULT_FILTER, NULL) == SOLCLIENT_FAIL)
    {
        return SOLCLIENT_FAIL;
    }
    if (solClient_context_create(SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                 &context_p, &contextFuncInfo, sizeof(contextFuncInfo)) == SOLCLIENT_FAIL)
    {
        return SOLCLIENT_FAIL;
    }
    sessionFuncInfo.rxMsgInfo.callback_p = sessionMessageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = sessionEventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;
    return SOLCLIENT_OK;
}

/* Configure the Session properties. */
solClient_returnCode_t SolaceConfigure(char *sessionProps[], RedisModuleString *connectionName)
{
    const char *connectionProps[50] = {0, };
    int propIndex = 0;
    char *username,*password,*vpnname,*host;

    /* Configure the Session properties. */
    propIndex = 0;

    host = sessionProps[3];
    vpnname = sessionProps[5];
    username = sessionProps[7];
    password =sessionProps[9];

    connectionProps[propIndex++] = SOLCLIENT_SESSION_PROP_HOST;
    connectionProps[propIndex++] = host;

    connectionProps[propIndex++] = SOLCLIENT_SESSION_PROP_VPN_NAME;
    connectionProps[propIndex++] = vpnname;

    connectionProps[propIndex++] = SOLCLIENT_SESSION_PROP_USERNAME;
    connectionProps[propIndex++] = username;

    connectionProps[propIndex++] = SOLCLIENT_SESSION_PROP_PASSWORD;
    connectionProps[propIndex++] = password;

    connectionProps[propIndex++] = NULL;
    solClient_opaqueSession_pt session_p;
    solClient_returnCode_t rc= solClient_session_create((char **)connectionProps,
                                    context_p,
                                    &session_p, &sessionFuncInfo, sizeof(sessionFuncInfo));
    if(rc==SOLCLIENT_OK){
        RedisModule_DictSet(sessionsMap, connectionName, session_p);
    }
}

solClient_returnCode_t SolaceDisconnect(RedisModuleString *connectionName)
{
    int * noKey;
    solClient_opaqueSession_pt session_p=RedisModule_DictGet(sessionsMap,connectionName,noKey);
    return solClient_session_disconnect(session_p);
}

solClient_returnCode_t SolaceUnsubscribe(RedisModuleString *connectionName, char *topicName)
{
    int * noKey;
    solClient_opaqueSession_pt session_p=RedisModule_DictGet(sessionsMap,connectionName,noKey);
    return solClient_session_topicUnsubscribeExt(session_p,
                                                 SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                                 topicName);
}

solClient_returnCode_t SolaceConnect(RedisModuleString *connectionName)
{
    int * noKey;
    solClient_opaqueSession_pt session_p=RedisModule_DictGet(sessionsMap,connectionName,noKey);
    return solClient_session_connect(session_p);
}

solClient_returnCode_t SolaceSubscribe(RedisModuleString *connectionName, char *topicName)
{
    int * noKey;
    solClient_opaqueSession_pt session_p=RedisModule_DictGet(sessionsMap,connectionName,noKey);
    return solClient_session_topicSubscribeExt(session_p,
                                               SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                               topicName);
}

solClient_returnCode_t SolaceSend(RedisModuleString *connectionName, char *topicName, char *message)
{
    int * noKey;
    solClient_opaqueSession_pt session_p=RedisModule_DictGet(sessionsMap,connectionName,noKey);
    solClient_opaqueMsg_pt msg_p = NULL;
    solClient_destination_t destination;
    solClient_msg_alloc ( &msg_p );
    solClient_msg_setDeliveryMode ( msg_p, SOLCLIENT_DELIVERY_MODE_DIRECT );
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = topicName;
    solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) );
    solClient_msg_setBinaryAttachment ( msg_p, message, ( solClient_uint32_t ) strlen (message) );
    solClient_returnCode_t statusCode = solClient_session_sendMsg ( session_p, msg_p );
    solClient_msg_free ( &msg_p );
    return statusCode;
}

/*
* SOLACE.CONFIGURE <name> HOST <solace_host> VPN <solace_vpn> USERNAME <username> PASSWORD <passsword>
* Configure connection to solace appliance 
*/
int SolaceConfigureCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (argc < 10)
    {
        return RedisModule_WrongArity(ctx);
    }
    RedisModule_AutoMemory(ctx);
    char *parsedArray[argc - 1];

    ParseModuleArgsAsStrings(argv, argc, parsedArray);
    char * connectionName = parsedArray[1];
    if (SolaceConfigure(parsedArray, RedisModule_CreateString(ctx, connectionName, strlen(connectionName))) == SOLCLIENT_FAIL)
    {
        RedisModule_ReplyWithError(ctx, "Could not configure solace connector");
        return REDISMODULE_ERR;
    }
    RedisModule_ReplyWithSimpleString(ctx, "Solace client configured");
    return REDISMODULE_OK;
}

/*
* SOLACE.CONNECT <name>
* Connects to solace appliance with configuration stored as <name>
*/
int SolaceConnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (argc < 2)
    {
        return RedisModule_WrongArity(ctx);
    }
    RedisModule_AutoMemory(ctx);

    char *parsedArray[argc - 1];
    ParseModuleArgsAsStrings(argv, argc, parsedArray);

    char *connectionName = parsedArray[1];

    SolaceConnect(RedisModule_CreateString(ctx, connectionName, strlen(connectionName)));
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

/*
 * SOLACE.DISCONNECT <name>
 * Disconnects from connection stored under <name> 
 */
int SolaceDisconnectCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{   
    if (argc < 2)
    {
        return RedisModule_WrongArity(ctx);
    }
    RedisModule_AutoMemory(ctx);
    char *parsedArray[argc - 1];
    ParseModuleArgsAsStrings(argv, argc, parsedArray);

    char *connectionName = parsedArray[1];

    SolaceDisconnect(RedisModule_CreateString(ctx, connectionName, strlen(connectionName)));
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

/*
 * SOLACE.SEND <name> <topic> <message>
 * Sends <message> via solace to topic <topic> with connection <name>
 */
int SolaceSendCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (argc < 4)
    {
        return RedisModule_WrongArity(ctx);
    }
    RedisModule_AutoMemory(ctx);
    char *parsedArray[argc - 1];
    ParseModuleArgsAsStrings(argv, argc, parsedArray);

    char *connectionName = parsedArray[1];
    char *topicName = parsedArray[2];
    char *message = parsedArray[3];

    if (SolaceSend(RedisModule_CreateString(ctx, connectionName, strlen(connectionName)), topicName, message) == SOLCLIENT_FAIL)
    {
        RedisModule_ReplyWithError(ctx, "Could not send message to solace");
        return REDISMODULE_ERR;
    }
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

/*
* SOLACE.SUBSCRIBE <name> <topic> 
* Subscribes to solace topic <topic> and listens to incoming messages via connectionn <name>
* Every incoming message will be stored in redis as string with <incoming_topic> as key and string mesasge payload as value
*/
int SolaceSubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (argc < 3)
    {
        return RedisModule_WrongArity(ctx);
    }

    RedisModule_AutoMemory(ctx);
    char *parsedArray[argc - 1];
    ParseModuleArgsAsStrings(argv, argc, parsedArray);

    char *connectionName = parsedArray[1];
    char* topic = parsedArray[2];

    SolaceSubscribe(RedisModule_CreateString(ctx, connectionName, strlen(connectionName)), topic);
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

/*
* SOLACE.UNSUBSCRIBE <name> <topic>
* Unsubscribes from specified solace topic for connection <name>
*/
int SolaceUnsubscribeCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc)
{
    if (argc < 3)
    {
        return RedisModule_WrongArity(ctx);
    }

    RedisModule_AutoMemory(ctx);
    char *parsedArray[argc - 1];
    ParseModuleArgsAsStrings(argv, argc, parsedArray);

    char *connectionName = parsedArray[1];
    char* topic = parsedArray[2];

    SolaceUnsubscribe(RedisModule_CreateString(ctx, connectionName, strlen(connectionName)), topic);
    RedisModule_ReplyWithSimpleString(ctx, "PASS");
    return REDISMODULE_OK;
}

//Main method to load module
int RedisModule_OnLoad(RedisModuleCtx *ctx)
{

    // Register the module itself
    if (RedisModule_Init(ctx, REDIS_SOLACE_MODULE_NAME, REDIS_SOLACE_MODULE_VERSION, REDISMODULE_APIVER_1) ==
        REDISMODULE_ERR)
    {
        return REDISMODULE_ERR;
    }

    //init solace
    if (SolaceInit() == SOLCLIENT_FAIL)
    {
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
    // Solace unsubscribe
    RMUtil_RegisterWriteCmd(ctx, SOLACE_UNSUBSCRIBE_COMMAND, SolaceUnsubscribeCommand);
    // Dict of active solace connections
    sessionsMap = RedisModule_CreateDict(NULL);
    return REDISMODULE_OK;
}
