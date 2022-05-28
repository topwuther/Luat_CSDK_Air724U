#include <stdio.h>
#include "string.h"
#include "at_process.h"
#include "at_tok.h"
#include "iot_os.h"
#include "iot_debug.h"
#include "iot_pmd.h"
#include "iot_network.h"
#include "iot_socket.h"
#include "httpclient.h"
#include "iot_vat.h"
#include "am_openat_httpclient.h"


#define SOCKET_MSG_NETWORK_READY (0)
#define SOCKET_MSG_NETWORK_LINKED (1)
#define HEAD_ACCEPT_KEY "Accept"
#define HEAD_ACCEPT_VALUE "*/*"
#define HEAD_ACCEPT_L_KEY "Accept-Language"
#define HEAD_ACCEPT_L_VALUE "cn"
#define HEAD_USER_KEY "User-Agent"
#define HEAD_USER_VALUE "*Mozilla/4.0"
#define HEAD_CONNECTION_KEY "Connection"
#define HEAD_CONNECTION_VALUE "Keep-Alive"

typedef struct {
    UINT8 type;
    UINT8 data;
}DEMO_NETWORK_MESSAGE;

static HANDLE g_s_http_task;
static char WIFILOC_IMEI[16] = {0};
static int cellid;  //cell ID
static int lac;  //LAC
static int mcc;  //MCC
static int mnc;  //MNC

static bool gsmGetIMEI(char* imeiOut)
{
    int err;
    ATResponse *p_response = NULL;
    char* line = NULL;
    //UINT8 index = 0;
    bool result = FALSE;
    if(!imeiOut)
    {
      return result;
    }

    err = at_send_command_numeric("AT+GSN", &p_response);
    if (err < 0 || p_response->success == 0){
      result = FALSE;
      goto end;
    }

    line = p_response->p_intermediates->line;

    {
      strcpy(imeiOut,line);
    }
    result = TRUE;
  end:
    at_response_free(p_response);
    return result;

}

static BOOL gsmGetCellInfo(int* mcc,int* mnc,int* lac,int* ci)
{
    ATResponse *p_response = NULL;
    bool result = FALSE;
	bool   lte;
	int i;
	char* out;

	//+CCED:GSM current cell info:460,00,550b,3c94,26,37,37,13
	//+CCED:LTE current cell:460,00,460045353407374,0,8,n50,3683,139024552,57,19,21771,42,471
	
    int err = at_send_command_singleline("AT+CCED=0,1", "+CCED:", &p_response);
    if (err < 0 || p_response->success == 0)
    {
        iot_debug_print("[iot_network] at_send_command_singleline error %d",__LINE__);
        goto end;
    }
    char* line = p_response->p_intermediates->line;  
    err = at_tok_start(&line);
    if (err < 0)
        goto end;
	if(strstr(line, "GSM"))
	{
		lte = FALSE;
	}
	else
	{
		lte = TRUE;
	}
    err = at_tok_start(&line);
	if (err < 0)
        goto end;
	err = at_tok_nextint(&line, mcc);
    if (err < 0)
        goto end;
	err = at_tok_nextint(&line, mnc);
    if (err < 0)
        goto end;
	if(lte)
	{			
		for(i = 0; i < 5; i++)
		{
			at_tok_nextstr(&line, &out);
		}
		err = at_tok_nextint(&line, ci);
		if (err < 0)
        	goto end;
		for(i = 0; i < 2; i++)
		{
			at_tok_nextstr(&line, &out);
		}
		err = at_tok_nextint(&line, lac);
		if (err < 0)
        	goto end;
	}
	else
	{			
		err = at_tok_nexthexint(&line, &lac);
	    if (err < 0)
        	goto end;
		err = at_tok_nexthexint(&line, &ci);
	    if (err < 0)
        	goto end;
	}
    result = TRUE;
end:              
    if(p_response!=NULL)
    {
        at_response_free(p_response);
        p_response=NULL;
    }  
    return result;
}

static void get_wifilocinfo(CHAR* http_url)
{
	CHAR* p = NULL;
	p = http_url;
	p += sprintf(p,"http://bs.openluat.com/cps_all?cell=%d,%d,%d,%d,%d&macs=", mcc, mnc, lac, cellid, 0);

	OPENAT_wifiScanRequest wreq = {0};
	wreq.max = 10;
	wreq.maxtimeout = 300;
	OPENAT_wifiApInfo* aps = (OPENAT_wifiApInfo*)iot_os_malloc(wreq.max * sizeof(OPENAT_wifiApInfo));
	wreq.aps = aps;
	iot_wifi_scan(&wreq);
	for (u32 i = 0; i < wreq.found; i++)
	{
		OPENAT_wifiApInfo *w = &wreq.aps[i];
		iot_debug_print("[wifiloc] found ap - {mac address: %x%lx, rssival: %d dBm, channel: %u}",
			 w->bssid_high, w->bssid_low, w->rssival, w->channel);
		p += sprintf(p,"%x%lx,%d,;",w->bssid_high, w->bssid_low, w->rssival);
	}
	iot_os_free(aps);
	p = p - 1; 
	p += sprintf(p,"&imei=%s",WIFILOC_IMEI);

	iot_debug_print("[wifiloc] data = %s", http_url);
}

static void wifiloc_print(char* pData)
{
	int status = -1;
	char* p = NULL;
	p = pData;
	if(!strncmp(p, "status=", strlen("status=")))
	{
		p +=  strlen("status=");
		status = atoi(&p[0]);
	}
	p += 2;
	if(0 == status )
	{
		iot_debug_print("[wifiloc]info: %s", p);
	}
	else
	{
		iot_debug_print("[wifiloc]error: %d", status);
	}

	
}

static void demo_network_connetck(void)
{
    T_OPENAT_NETWORK_CONNECT networkparam;
    
    memset(&networkparam, 0, sizeof(T_OPENAT_NETWORK_CONNECT));
    memcpy(networkparam.apn, "CMNET", strlen("CMNET"));

    iot_network_connect(&networkparam);

}

static void demo_networkIndCallBack(E_OPENAT_NETWORK_STATE state)
{
    DEMO_NETWORK_MESSAGE* msgptr = iot_os_malloc(sizeof(DEMO_NETWORK_MESSAGE));
    iot_debug_print("[wifiloc] network ind state %d", state);
    if(state == OPENAT_NETWORK_LINKED)
    {
        msgptr->type = SOCKET_MSG_NETWORK_LINKED;
        iot_os_send_message(g_s_http_task, (PVOID)msgptr);
        return;
    }
    else if(state == OPENAT_NETWORK_READY)
    {
        msgptr->type = SOCKET_MSG_NETWORK_READY;
        iot_os_send_message(g_s_http_task,(PVOID)msgptr);
        return;
    }
    iot_os_free(msgptr);
}

void http_debug(const char * fun,const char* data,UINT32 len ,char * fmt, ...)
{
  va_list ap;
  char fmtString[128] = {0};
  UINT16 fmtStrlen;
  strcat(fmtString, "[http]--");
  strcat(fmtString, fun);
  strcat(fmtString, "--");

  fmtStrlen = strlen(fmtString);
  va_start (ap, fmt);
  fmtStrlen += vsnprintf(fmtString+fmtStrlen, sizeof(fmtString)-fmtStrlen, fmt, ap);
  va_end (ap);

  if(fmtStrlen != 0)
  {
      iot_debug_print("%s", fmtString);
  }
}



void get_wifiloc(void)
{
  HTTP_SESSION_HANDLE pHTTP;
  CHAR readBuff[1460];
  UINT32 readSize = 0;
  UINT32 readTotalLen = 0;
  CHAR token[32];
  UINT32 tokenSize=32;
  UINT32 nRetCode;
  CHAR http_url[256] = {0};


  pHTTP = HTTPClientOpenRequest(0);
  
  HTTPClientSetDebugHook(pHTTP, http_debug);

  if (HTTPClientSetVerb(pHTTP,VerbGet) != HTTP_CLIENT_SUCCESS)
  {
    iot_debug_print("[wifiloc] HTTPClientSetVerb error");
    return;
  }

  if((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_ACCEPT_KEY, HEAD_ACCEPT_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
  {
    return;
  }
  if((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_ACCEPT_L_KEY, HEAD_ACCEPT_L_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
  {
    return;
  }
  if((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_USER_KEY, HEAD_USER_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
  {
    return;
  }
  if((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_CONNECTION_KEY, HEAD_CONNECTION_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
  {
    return;
  }

  get_wifilocinfo(http_url);
  
  iot_debug_print("[wifiloc] HTTPClientSendRequest enter");
  if (HTTPClientSendRequest(pHTTP,http_url, NULL, 0,TRUE,0,0) != HTTP_CLIENT_SUCCESS ) 
  {
    iot_debug_print("[wifiloc] HTTPClientSendRequest error");
    return;
  }
  iot_debug_print("[wifiloc] HTTPClientRecvResponse enter");

  if(HTTPClientRecvResponse(pHTTP,20000) != HTTP_CLIENT_SUCCESS)
  {
    iot_debug_print("[wifiloc] HTTPClientRecvResponse error");
    return;
  }
  
  if((nRetCode = HTTPClientFindFirstHeader(pHTTP, "content-length", token, &tokenSize)) != HTTP_CLIENT_SUCCESS)
  {
    iot_debug_print("[wifiloc] HTTPClientFindFirstHeader error");
    return;
  }
  else
  {
    iot_debug_print("[wifiloc] HTTPClientFindFirstHeader %d,%s", tokenSize, token);
  }
  HTTPClientFindCloseHeader(pHTTP);

  while(nRetCode == HTTP_CLIENT_SUCCESS || nRetCode != HTTP_CLIENT_EOS)
  {
      // Set the size of our buffer
      
      // Get the data
      nRetCode = HTTPClientReadData(pHTTP,readBuff,sizeof(readBuff),300,&readSize);

      readTotalLen += readSize;
      if(nRetCode != HTTP_CLIENT_SUCCESS || nRetCode == HTTP_CLIENT_EOS)
      {
        iot_debug_print("[wifiloc] HTTPClientReadData end nRetCode %d", nRetCode);
		wifiloc_print(readBuff);
        break;
      }

      iot_debug_print("[wifiloc] HTTPClientReadData readTotalLen %d, %d, nRetCode %d", readTotalLen, readSize, nRetCode);
  }

  if(HTTPClientCloseRequest(&pHTTP) != HTTP_CLIENT_SUCCESS)
  {
    iot_debug_print("[wifiloc] HTTPIntrnConnectionClose error");
    return;
  }
}

static void demo_http_task(PVOID pParameter)
{
    DEMO_NETWORK_MESSAGE*    msg;
    iot_debug_print("[wifiloc] wait network ready....");

    while(1)
    {
        iot_os_wait_message(g_s_http_task, (PVOID)&msg);

        switch(msg->type)
        {
            case SOCKET_MSG_NETWORK_READY:
                iot_debug_print("[wifiloc] network connecting....");
                demo_network_connetck();
                break;
            case SOCKET_MSG_NETWORK_LINKED:
                iot_debug_print("[wifiloc] network connected");
				gsmGetCellInfo(&mcc, &mnc, &lac, &cellid);
				iot_debug_print("[wifiloc] mnc: %d, mcc: %d, la:%d, ci:%d",mcc, mnc, lac, cellid);
				gsmGetIMEI(WIFILOC_IMEI);
				iot_debug_print("[wifiloc] WIFILOC_IMEI: %s",WIFILOC_IMEI);
                get_wifiloc();
                break;
        }

        iot_os_free(msg);
    }
}

void demo_http_init(void)
{ 
  iot_debug_print("[wifiloc] demo_http_init");

  //×¢²áÍøÂç×´Ì¬»Øµ÷º¯Êý
  iot_network_set_cb(demo_networkIndCallBack);

  g_s_http_task = iot_os_create_task(demo_http_task,
                      NULL,
                      4096,
                      5,
                      OPENAT_OS_CREATE_DEFAULT,
                      "demo_http");
}

int appimg_enter(void *param)
{
    iot_os_sleep(400);
	iot_debug_print("[wifiloc] app_main");
	demo_http_init();
    return 0;
}

void appimg_exit(void)
{
    iot_debug_print("[wifiloc] application image exit");
}

