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

typedef struct
{
	UINT8 type;
	UINT8 data;
} DEMO_NETWORK_MESSAGE;

static HANDLE g_s_http_task;
static int cellid; //cell ID
static int lac;	   //LAC
static int mcc;	   //MCC
static int mnc;	   //MNC

static BOOL gsmGetCellInfo(int *mcc, int *mnc, int *lac, int *ci)
{
	ATResponse *p_response = NULL;
	bool result = FALSE;
	bool lte;
	int i;
	char *out;

	//+CCED:GSM current cell info:460,00,550b,3c94,26,37,37,13
	//+CCED:LTE current cell:460,00,460045353407374,0,8,n50,3683,139024552,57,19,21771,42,471

	int err = at_send_command_singleline("AT+CCED=0,1", "+CCED:", &p_response);
	if (err < 0 || p_response->success == 0)
	{
		iot_debug_print("[coreTest-False-gsmloc]: at_send_command_singleline error %d", __LINE__);
		goto end;
	}
	char *line = p_response->p_intermediates->line;
	err = at_tok_start(&line);
	if (err < 0)
		goto end;
	if (strstr(line, "GSM"))
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
	if (lte)
	{
		for (i = 0; i < 5; i++)
		{
			at_tok_nextstr(&line, &out);
		}
		err = at_tok_nextint(&line, ci);
		if (err < 0)
			goto end;
		for (i = 0; i < 2; i++)
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
	if (p_response != NULL)
	{
		at_response_free(p_response);
		p_response = NULL;
	}
	return result;
}

static void get_gsmlocinfo(CHAR *http_url)
{
	CHAR *p = NULL;
	p = http_url;
	p += sprintf(p, "http://bs.openluat.com/cps?cell=%d,%d,%d,%d,%d", mcc, mnc, lac, cellid, 0);

	iot_debug_print("[coreTest-gsmloc]: data = %s", http_url);
}

static void gsmloc_print(char *pData)
{
	int status = -1;
	char *p = NULL;
	p = pData;
	if (!strncmp(p, "status=", strlen("status=")))
	{
		p += strlen("status=");
		status = atoi(&p[0]);
	}
	p += 2;
	if (0 == status)
	{
		iot_debug_print("[coreTest-gsmloc]:info: %s", p);
	}
	else
	{
		iot_debug_print("[coreTest-False-gsmloc]:error: %d", status);
	}
}

static void http_debug(const char *fun, const char *data, UINT32 len, char *fmt, ...)
{
	va_list ap;
	char fmtString[128] = {0};
	UINT16 fmtStrlen;
	strcat(fmtString, "[http]--");
	strcat(fmtString, fun);
	strcat(fmtString, "--");

	fmtStrlen = strlen(fmtString);
	va_start(ap, fmt);
	fmtStrlen += vsnprintf(fmtString + fmtStrlen, sizeof(fmtString) - fmtStrlen, fmt, ap);
	va_end(ap);

	if (fmtStrlen != 0)
	{
		iot_debug_print("%s", fmtString);
	}
}

void get_gsmloc(void)
{
	HTTP_SESSION_HANDLE pHTTP;
	CHAR readBuff[1460];
	UINT32 readSize = 0;
	UINT32 readTotalLen = 0;
	CHAR token[32];
	UINT32 tokenSize = 32;
	UINT32 nRetCode;
	CHAR http_url[256] = {0};

	pHTTP = HTTPClientOpenRequest(0);

	HTTPClientSetDebugHook(pHTTP, http_debug);

	if (HTTPClientSetVerb(pHTTP, VerbGet) != HTTP_CLIENT_SUCCESS)
	{
		iot_debug_print("[coreTest-False-gsmloc]: HTTPClientSetVerb error");
		return;
	}

	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_ACCEPT_KEY, HEAD_ACCEPT_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
	{
		return;
	}
	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_ACCEPT_L_KEY, HEAD_ACCEPT_L_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
	{
		return;
	}
	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_USER_KEY, HEAD_USER_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
	{
		return;
	}
	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, HEAD_CONNECTION_KEY, HEAD_CONNECTION_VALUE, TRUE)) != HTTP_CLIENT_SUCCESS)
	{
		return;
	}

	get_gsmlocinfo(http_url);

	iot_debug_print("[coreTest-gsmloc]: HTTPClientSendRequest enter");
	if (HTTPClientSendRequest(pHTTP, http_url, NULL, 0, TRUE, 0, 0) != HTTP_CLIENT_SUCCESS)
	{
		iot_debug_print("[coreTest-False-gsmloc]: HTTPClientSendRequest error");
		return;
	}
	iot_debug_print("[coreTest-gsmloc]: HTTPClientRecvResponse enter");

	if (HTTPClientRecvResponse(pHTTP, 20000) != HTTP_CLIENT_SUCCESS)
	{
		iot_debug_print("[coreTest-False-gsmloc]: HTTPClientRecvResponse error");
		return;
	}

	if ((nRetCode = HTTPClientFindFirstHeader(pHTTP, "content-length", token, &tokenSize)) != HTTP_CLIENT_SUCCESS)
	{
		iot_debug_print("[coreTest-False-gsmloc]: HTTPClientFindFirstHeader error");
		return;
	}
	else
	{
		iot_debug_print("[coreTest-gsmloc]: HTTPClientFindFirstHeader %d,%s", tokenSize, token);
	}
	HTTPClientFindCloseHeader(pHTTP);

	while (nRetCode == HTTP_CLIENT_SUCCESS || nRetCode != HTTP_CLIENT_EOS)
	{
		// Set the size of our buffer

		// Get the data
		nRetCode = HTTPClientReadData(pHTTP, readBuff, sizeof(readBuff), 300, &readSize);

		readTotalLen += readSize;
		if (nRetCode != HTTP_CLIENT_SUCCESS || nRetCode == HTTP_CLIENT_EOS)
		{
			iot_debug_print("[coreTest-gsmloc]: HTTPClientReadData end nRetCode %d", nRetCode);
			gsmloc_print(readBuff);
			break;
		}

		iot_debug_print("[coreTest-gsmloc]: HTTPClientReadData readTotalLen %d, %d, nRetCode %d", readTotalLen, readSize, nRetCode);
	}

	if (HTTPClientCloseRequest(&pHTTP) != HTTP_CLIENT_SUCCESS)
	{
		iot_debug_print("[coreTest-False-gsmloc]: HTTPIntrnConnectionClose error");
		return;
	}
}

bool gsmlocTest(void)
{
	extern bool networkstatus;
	if (networkstatus == FALSE)
		return FALSE;
	if(gsmGetCellInfo(&mcc, &mnc, &lac, &cellid)==FALSE)
		return FALSE;
	iot_debug_print("[wifiloc] mnc: %d, mcc: %d, la:%d, ci:%d", mcc, mnc, lac, cellid);
	get_gsmloc();
}
