#include "solclient/solClient.h"
#include "solclient/solClientMsg.h"
#include "solace.h"

/* Message Count */
static int msgCount = 0;

/*****************************************************************************
 * sessionMessageReceiveCallback
 *
 * The message callback is invoked for each Direct message received by
 * the Session. In this sample, the message is printed to the screen.
 *****************************************************************************/
solClient_rxMsgCallback_returnCode_t sessionMessageReceiveCallback ( solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p ){
    printf ( "Received message:\n" );
    solClient_msg_dump ( msg_p, NULL, 0 );
    printf ( "\n" );

    msgCount++;

    return SOLCLIENT_CALLBACK_OK;
}

/*****************************************************************************
 * sessionEventCallback
 *
 * The event callback function is mandatory for session creation.
 *****************************************************************************/
void sessionEventCallback ( solClient_opaqueSession_pt opaqueSession_p,
                solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p ){

}

solClient_returnCode_t SolaceInit(){
    if(solClient_initialize ( SOLCLIENT_LOG_DEFAULT_FILTER, NULL )==SOLCLIENT_FAIL){
        return SOLCLIENT_FAIL;
    }
    if(solClient_context_create ( SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                               &context_p, &contextFuncInfo, sizeof ( contextFuncInfo ) )==SOLCLIENT_FAIL){
                                    return SOLCLIENT_FAIL;
                               }
    sessionFuncInfo.rxMsgInfo.callback_p = sessionMessageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = sessionEventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;
    return SOLCLIENT_OK;
}

/* Configure the Session properties. */
solClient_returnCode_t SolaceConfigure(char* sessionProps[] ){
    return solClient_session_create ( ( char ** ) sessionProps,
                               context_p,
                               &session_p, &sessionFuncInfo, sizeof ( sessionFuncInfo ) );
}

solClient_returnCode_t SolaceDisconnect(){
    return solClient_cleanup (  );
}

solClient_returnCode_t SolaceUnsubscribe(char* topicName){
     return solClient_session_topicUnsubscribeExt ( session_p,
                                            SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                            topicName );
}

solClient_returnCode_t SolaceConnect(){
    return solClient_session_connect ( session_p );
}


solClient_returnCode_t SolaceSubscribe(char* topicName){
    return solClient_session_topicSubscribeExt ( session_p,
                                          SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                          topicName );
}

/*****************************************************************************
 * main
 * 
 * The entry point to the application.
 *****************************************************************************/
int
main ( int argc, char *argv[] )
{
    char* props[]={0,};

    SolaceInit();
    SolaceConfigure(props);
    SolaceConnect();
    SolaceSubscribe("");
    SolaceDisconnect();
    return 0;
}
