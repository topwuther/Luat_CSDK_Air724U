/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_audio.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/10/21
 *
 * Description:
 *          音频接口
 * History:
 *     panjun 2015.05.06 Optimize audio's API.
 *     panjun 2015.04.30 Add audio's API according to MTK.
 **************************************************************************/
#ifdef LUA_AUDIO_LIB

#include "string.h"
#include "stdio.h"
#include "lplatform.h"
#include "platform_malloc.h"
#include "platform_audio.h"
#include "am_openat.h"
static u8 *audplaybuffer = NULL;

static BOOL is_recording = FALSE;

#define gpio_802pa  14
static BOOL is_802pa_init = FALSE;

extern WCHAR* strtows(WCHAR* dst, const char* src);

static void audio_play_callback(E_AMOPENAT_PLAY_ERROR result)
{
    PlatformMsgData rtosmsg;

	#if 0
    if(audplaybuffer)
    {
        platform_free(audplaybuffer);
        audplaybuffer = NULL;
    }
    #endif
	PUB_TRACE("audio play error: %d", result);
    if(OPENAT_AUD_PLAY_ERR_NO == result || OPENAT_AUD_PLAY_ERR_END_OF_FILE == result)
    {
       	rtosmsg.audioData.playEndInd = TRUE;
		rtosmsg.audioData.playErrorInd = FALSE;
    }
    else
    {
	 	rtosmsg.audioData.playEndInd = FALSE;
        rtosmsg.audioData.playErrorInd = TRUE;
    }

    platform_rtos_send(MSG_ID_RTOS_AUDIO, &rtosmsg);

}
/*+\new\wj\2020.4.26\实现录音接口*/
/*+\BUG\wangyuan\2020.07.31\BUG_2736:CSDK 大唐对讲机需求 支持流录音*/
static void audio_stream_record_cb(int event, char* data, int len)
/*-\BUG\wangyuan\2020.07.31\BUG_2736:CSDK 大唐对讲机需求 支持流录音*/
{
	PlatformMsgData rtosmsg;
	rtosmsg.streamRecordLen = len;
	platform_rtos_send(MSG_ID_ROTS_STRAM_RECORD_IND, &rtosmsg);
}
/*-\new\wj\2020.4.26\实现录音接口*/
static void audio_record_callback(E_AMOPENAT_RECORD_ERROR result)
{
    PlatformMsgData rtosmsg;

    if(!is_recording)
    {
        return;
    }
    
    if(OPENAT_AUD_REC_ERR_NONE == result)
    {
        rtosmsg.recordData.recordEndInd= TRUE;
        rtosmsg.recordData.recordErrorInd = FALSE;
    }
    else
    {
        PUB_TRACE("audio record error: %d", result);
        rtosmsg.recordData.recordEndInd = FALSE;
        rtosmsg.recordData.recordErrorInd = TRUE;
    }

    platform_rtos_send(MSG_ID_RTOS_RECORD, &rtosmsg);

    //is_recording = FALSE;
}


static E_AMOPENAT_AUD_FORMAT getFileFormat(const char *filename)
{
    if(strstr(filename,".amr") || strstr(filename,".AMR"))
    {
       return OPENAT_AUD_FORMAT_AMRNB;//OPENAT_AUD_PLAY_FILE_FORMAT_AMR;
    } 
    else if(strstr(filename,".mp3") || strstr(filename,".MP3"))
    {
        return OPENAT_AUD_FORMAT_MP3;
    }
	else if(strstr(filename,".pcm") || strstr(filename,".PCM"))
    {
        return OPENAT_AUD_FORMAT_PCM;
    }
	#if 0
    else if(strstr(filename,".mid") || strstr(filename,".MID"))
    {
        return -1;//OPENAT_AUD_PLAY_FILE_FORMAT_MIDI;
    }
	#endif
    else
    {
        PUB_TRACE("getFileFormat(ERROR) filename: %s.", filename);
    }
    return -1;
}

static E_AMOPENAT_PLAY_MODE getDataFormat(PlatformAudioFormat audFormat)
{
	#if 0
    static const E_AMOPENAT_PLAY_MODE mode[NumOfPlatformAudFormats] =
    {
    	/*+\BUG\wangyuan\2020.12.31\BUG_4041:3024LUA版本录音demo录音功能没有作用*/
		OPENAT_AUD_FORMAT_MP3,
        OPENAT_AUD_FORMAT_PCM,
        OPENAT_AUD_FORMAT_WAVPCM,
        OPENAT_AUD_FORMAT_AMRNB,
        OPENAT_AUD_FORMAT_AMRWB,
        /*-\BUG\wangyuan\2020.12.31\BUG_4041:3024LUA版本录音demo录音功能没有作用*/
		/*+\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
        OPENAT_AUD_FORMAT_SPEEX,
		/*-\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
        //OPENAT_AUD_PLAY_MODE_MIDI,
    };

    if(audFormat < NumOfPlatformAudFormats)
        return mode[audFormat];
    else
        return OPENAT_AUD_FORMAT_QTY;
	#endif
	
	switch(audFormat)
	{
		case PLATFORM_AUD_MP3: return OPENAT_AUD_FORMAT_MP3;
		case PLATFORM_AUD_PCM: return OPENAT_AUD_FORMAT_PCM; 
		case PLATFORM_AUD_WAV: return OPENAT_AUD_FORMAT_WAVPCM;
		case PLATFORM_AUD_AMRNB: return OPENAT_AUD_FORMAT_AMRNB;
		case PLATFORM_AUD_AMRWB: return OPENAT_AUD_FORMAT_AMRWB;
		/*+\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
		case PLATFORM_AUD_SPEEX: return OPENAT_AUD_FORMAT_SPEEX;
		/*-\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
		default: return OPENAT_AUD_FORMAT_QTY;
	}
}


void am_pm802_pa_init(void)
{
    T_AMOPENAT_GPIO_CFG cfg = {0};

	cfg.mode = OPENAT_GPIO_OUTPUT;
	cfg.param.defaultState = 0;

    OPENAT_config_gpio(gpio_802pa, &cfg);
}

void platform_audio_play_callback(const char *file_name,int ret)
{

	PlatformMsgData rtosmsg;

	//OPENAT_set_gpio(gpio_802pa,0);	

	if(ret == 0)
	{
		rtosmsg.audioData.playEndInd = TRUE;
		rtosmsg.audioData.playErrorInd = FALSE;
	}
	else
	{
		PUB_TRACE("audio play error: %d", ret);
		rtosmsg.audioData.playEndInd = FALSE;
		rtosmsg.audioData.playErrorInd = TRUE;
	}

	platform_rtos_send(MSG_ID_RTOS_AUDIO, &rtosmsg);

}

int platform_audio_play(AudioPlayParam *param)
{
    T_AMOPENAT_PLAY_PARAM playParam;
    static WCHAR uniFileName[100];

    E_AMOPENAT_AUD_FORMAT fileformat;
    char *name = NULL;

    FILE *fp;
	
	playParam.playBuffer = param->isBuffer;

	if(param->isBuffer)
	{
		playParam.playBufferParam.pBuffer = param->u.buffer.data;
		playParam.playBufferParam.len = param->u.buffer.len;
		playParam.playBufferParam.format = getDataFormat(param->u.buffer.format);
		playParam.playBufferParam.loop = param->u.buffer.loop;
		playParam.playBufferParam.callback = audio_play_callback;

		PUB_TRACE("platform_audio_play format:%d,len:%d",playParam.playBufferParam.format,playParam.playBufferParam.len);
		if(playParam.playBufferParam.format == OPENAT_AUD_FORMAT_QTY)
		{
	        PUB_TRACE("platform_audio_play:unknown format");
	        return PLATFORM_ERR;
	    }
	}else
	{
		PUB_TRACE("platform_audio_play param->u.filename:%s",param->u.filename);
		if(strlen(param->u.filename) > (sizeof(uniFileName)/sizeof(uniFileName[0])-1))
	    {
	        PUB_TRACE("platform_audio_play:filename too long %d.", strlen(param->u.filename));
	        return PLATFORM_ERR;
	    }

	    fileformat = getFileFormat(param->u.filename);

	    if(fileformat == -1)
	    {
	        PUB_TRACE("platform_audio_play:unknown format");
	        return PLATFORM_ERR;
	    }

		/*+\NEW\shenyuanyuan\2020.3.26\修改调用audiocore.play死机问题*/
		playParam.playFileParam.fileName = param->u.filename;

		playParam.playFileParam.fileFormat = fileformat;
		/*-\NEW\shenyuanyuan\2020.3.26\修改调用audiocore.play死机问题*/
		playParam.playFileParam.callback = audio_play_callback;
	}
        
    
#ifdef LUA_TODO
	IVTBL(set_play_callback)(audio_play_callback);
#endif
  
	if(IVTBL(play_music)(&playParam) < 0)
	{
		PUB_TRACE("platform_audio_play:play_music failed");
		return PLATFORM_ERR;
	}
              
    return PLATFORM_OK;
}

/*+\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/
int platform_audio_streamplay(PlatformAudioPlayType PlayType,PlatformAudioFormat format,char* data,int len)
/*-\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/
{
	E_AMOPENAT_AUD_FORMAT dataformat;

	dataformat = getDataFormat(format);
	if(dataformat == OPENAT_AUD_FORMAT_QTY)
	{
        PUB_TRACE("platform_audio_play:unknown format");
        return PLATFORM_ERR;
    }
	/*+\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/
    //PUB_TRACE("platform_audio_streamplay:PlatformAudioPlayType:%d PlatformAudioFormat:%d dataformat:%d" ,PlayType ,format,dataformat);	
	if(PlayType != PLATFORM_AUD_PLAY_TYPE_POC)
		return OPENAT_streamplay(dataformat,audio_play_callback,data,len);
	else
		return OPENAT_streamplayV2(PlayType,dataformat,audio_play_callback,data,len);
	/*-\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/	
}
/*+\bug\wj\2020.5.14\流播放问题PCM无上报，流式播放是同步阻塞接口不合适*/
int platform_audio_getStreamRemainDataLen()
{
	return OPENAT_StreamRemainDataLen();
}
/*-\bug\wj\2020.5.14\流播放问题PCM无上报，流式播放是同步阻塞接口不合适*/
int platform_audio_stop(void)
{

    IVTBL(stop_music)();

    return PLATFORM_OK;
}

/*+\NEW\zhuth\2014.7.25\新增设置音频通道和音量的同步接口*/
/*+\BUG\wangyuan\2020.11.27\BUG_3634：在Luat版本上开发“设置mic输入通道”的接口*/
int platform_audio_set_channel(PlatformAudioChannel outputchannel,PlatformMicChannel inputchannel)
{
	if(inputchannel == 0xff)
		inputchannel = 0;
	
    if(outputchannel >= NumOfPlatformAudChannels ||  inputchannel >= NumOfPlatformMicChannels)
    {
        PUB_TRACE("platform_audio_set_channel(PLATFORM_ERR) outputchannel=%d, inputchannel=%d.", outputchannel, inputchannel);
        return PLATFORM_ERR;
    }
    
    IVTBL(set_channel)(outputchannel, inputchannel);
/*-\BUG\wangyuan\2020.11.27\BUG_3634：在Luat版本上开发“设置mic输入通道”的接口*/
    return PLATFORM_OK;
}

/*+\NEW\xiongjunqun\2015.05.28\增加通话中调节音量接口*/
/*+\bug\wj\2020.5.6\增加通话中调节音量接口*/
int platform_audio_set_sph_vol(PlatformAudioVol vol)
{
	int playvol[NumOfPlatformAudVols] =
    {
        0,//PLATFORM_AUD_VOL0
		15,//PLATFORM_AUD_VOL1
        30,//PLATFORM_AUD_VOL2
        45,//PLATFORM_AUD_VOL3
        60,//PLATFORM_AUD_VOL4
        75,//PLATFORM_AUD_VOL5
        90,//PLATFORM_AUD_VOL6
        100//PLATFORM_AUD_VOL7
    };

    if(vol >= NumOfPlatformAudVols)
    {
        PUB_TRACE("platform_audio_set_sph_vol(PLATFORM_ERR) vol=%d.", vol);
        return PLATFORM_ERR;
    }
    
    IVTBL(set_sph_vol)(playvol[vol]);
    return PLATFORM_OK;
}
/*-\bug\wj\2020.5.6\增加通话中调节音量接口*/
/*-\NEW\xiongjunqun\2015.05.28\增加通话中调节音量接口*/

int platform_audio_set_vol(PlatformAudioVol vol)
{
	int playvol[NumOfPlatformAudVols] =
    {
        0,//PLATFORM_AUD_VOL0
		15,//PLATFORM_AUD_VOL1
        30,//PLATFORM_AUD_VOL2
        45,//PLATFORM_AUD_VOL3
        60,//PLATFORM_AUD_VOL4
        75,//PLATFORM_AUD_VOL5
        90,//PLATFORM_AUD_VOL6
        100//PLATFORM_AUD_VOL7
    };
    if(vol >= NumOfPlatformAudVols)
    {
        PUB_TRACE("platform_audio_set_vol(PLATFORM_ERR) vol=%d.", vol);
        return PLATFORM_ERR;
    }
    
    IVTBL(set_music_volume)(playvol[vol]);
    return PLATFORM_OK;
}

int platform_audio_set_mic_vol(PlatformMicVol vol)
{
    if(vol >15)
    {
        PUB_TRACE("platform_audio_set_mic_vol(PLATFORM_ERR) vol=%d.", vol);
        return PLATFORM_ERR;
    }
    IVTBL(set_mic_gain)(vol);
    return PLATFORM_OK;
}


int platform_audio_set_loopback(BOOL flag, PlatformAudioLoopback typ, BOOL setvol, u32 vol)
{
    if(typ >= NumOfPlatformAudLoopbacks)
    {
        PUB_TRACE("platform_audio_set_loopback(PLATFORM_ERR) typ=%d.", typ);
        return PLATFORM_ERR;
    }
    IVTBL(audio_loopback)(flag, typ, setvol, vol);
    return PLATFORM_OK;
}


/*+\new\wj\2020.4.26\实现录音接口*/
int platform_audio_record(char* file_name, int time_sec, int quality, PlatformAudioRecordType type, PlatformAudioFormat format)
{
	E_AMOPENAT_RECORD_PARAM param;
	E_AMOPENAT_AUD_FORMAT openat_format;

    if(is_recording)
    {
        return PLATFORM_ERR;
    }

	param.fileName = file_name;
	param.record_mode = OPENAT_RECORD_FILE;
	param.quality = (E_AMOPENAT_RECORD_QUALITY)quality;
	param.type = (E_AMOPENAT_RECORD_TYPE)type;

	/*+\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/
	PUB_TRACE("platform_audio_record type=%d format=%d" ,type ,format);

	openat_format = getDataFormat(format);

	// switch(format)
	// {
	// 	case 1: openat_format = OPENAT_AUD_FORMAT_PCM; break;
	// 	case 2: openat_format = OPENAT_AUD_FORMAT_WAVPCM;break;
	// 	case 3: openat_format = OPENAT_AUD_FORMAT_AMRNB;break;
	// 	/*+\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
	// 	case 4: openat_format = OPENAT_AUD_FORMAT_SPEEX;break;
	// 	/*-\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
	// 	default: return PLATFORM_ERR;
	// }
	/*-\NEW\czm\2020.11.13\bug:3271合并展锐降噪算法到通用版本*/
	param.format = openat_format;
	param.time_sec = time_sec;
	
    if(OPENAT_audio_record(&param,audio_record_callback) == 0)
	{
		is_recording = TRUE;
		return PLATFORM_OK;
	}
	return PLATFORM_ERR;
}
/*-\new\wj\2020.4.26\实现录音接口*/
/*+\new\wj\2020.4.26\实现录音接口*/
int platform_audio_stop_record(void)
{
    return OPENAT_audio_stop_record() == 0 ? PLATFORM_OK : PLATFORM_ERR;
}

int platform_audio_stream_record(int time_sec, int quality, int type, int format, 
	/*+\bug2241\zhuwangbin\2020.6.20\流录音可配置回调长度阀值*/
	int length
	/*-\bug2241\zhuwangbin\2020.6.20\流录音可配置回调长度阀值*/
	)
{
	E_AMOPENAT_RECORD_PARAM param;
	E_AMOPENAT_AUD_FORMAT openat_format;
		
    if(is_recording)
    {
        return PLATFORM_ERR;
    }
	
	param.record_mode = OPENAT_RECORD_STREAM;
	param.quality = (E_AMOPENAT_RECORD_QUALITY)quality;
	param.type = (E_AMOPENAT_RECORD_TYPE)type;
	switch(format)
	{
		case 1: openat_format = OPENAT_AUD_FORMAT_PCM; break;
		case 2: openat_format = OPENAT_AUD_FORMAT_WAVPCM;break;
		case 3: openat_format = OPENAT_AUD_FORMAT_AMRNB;break;
		/*+\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
		case 4: openat_format = OPENAT_AUD_FORMAT_SPEEX;break;
		/*-\NEW\zhuwangbin\2020.05.15\增加speex格式的录音和播放*/
		default: return PLATFORM_ERR;
	}
	param.format = openat_format;
	param.time_sec = time_sec;
	param.stream_record_cb = audio_stream_record_cb;
	/*+\bug2241\zhuwangbin\2020.6.20\流录音可配置回调长度阀值*/
    param.thresholdLength = length;
	/*-\bug2241\zhuwangbin\2020.6.20\流录音可配置回调长度阀值*/
    if(OPENAT_audio_record(&param,audio_record_callback) == 0)
    {
		is_recording = TRUE;
		return PLATFORM_OK;
	}

	return PLATFORM_ERR;
	//OPENAT_audio_record
}

int platform_audio_read_record_buf(char *readBuf,int readLen)
{
	if(!is_recording)
		return 0;
	return OPENAT_audio_stream_read_buf(readBuf,readLen);
}

int platform_audio_delete_record()
{
	is_recording = FALSE;
	OPENAT_delete_record();
	return 1;
}
/*-\new\wj\2020.4.26\实现录音接口*/
/*+\new\wj\2020.5.29\通话前可以播放音频，接通后还可以正常通话*/
BOOL platform_audio_start_voice()
{
	return OPENAT_audio_start_voice();
}
/*-\new\wj\2020.5.29\通话前可以播放音频，接通后还可以正常通话*/

/*+\new\zhuwangbin\2020.6.2\添加音频功放类型设置接口*/
int platform_setpa(PlatformSpkPaType type)
{
	return OPENAT_setpa((OPENAT_SPKPA_TYPE_T)type);
}

int platform_getpa(void)
{
	return OPENAT_getpa();
}
/*-\new\zhuwangbin\2020.6.2\添加音频功放类型设置接口*/

/*+\bug2767\zhuwangbin\2020.8.5\添加外部pa设置接口*/
int platform_setexpa(BOOL enable, UINT16 gpio, UINT16 count, 
					UINT16 us,  E_AMOPENAT_AUDIO_CHANNEL outDev)
{
	OPENAT_EXPA_T param;
	param.enable = enable;
	param.gpio = gpio;
	param.count = count;
	param.us = us;
	param.outDev = outDev;

	return OPENAT_ExPASet(&param);
}
/*-\bug2767\zhuwangbin\2020.8.5\添加外部pa设置接口*/

/*+\wj\new\2020.10.16\添加rtmp功能AT指令和lua使用接口*/
void platform_rtmp_callback(E_OPENAT_RTMP_RESULT_CODE code)
{
	PlatformRtmpData rtmpData;
	rtmpData.result = TRUE;
	rtmpData.result_code = (UINT8)code;
	platform_rtos_send(MSG_ID_RTOS_RTMP, &rtmpData);
}

BOOL platform_rtmp_open(const char* url)
{
	return OPENAT_rtmp_open_url(platform_rtmp_callback,url);
}

BOOL platform_rtmp_close()
{

	return OPENAT_rtmp_close();
	
}
/*-\wj\new\2020.10.16\添加rtmp功能AT指令和lua使用接口*/
/*+\NEW\zhuwangbin\2020.8.11\添加耳机插拔配置*/
int platform_headPlug(int type)
{
	if (type >= OPENAT_AUD_HEADSET_TYPE_QTY)
	{
		return -1;
	}
	
	return OPENAT_headPlug(type);
}
/*-\NEW\zhuwangbin\2020.8.11\添加耳机插拔配置*/
				
#endif

/*-\NEW\zhuth\2014.7.25\新增设置音频通道和音量的同步接口*/
/*+\new\wj\2020.9.19\lua添加耳机自动检测功能，添加开机和耳机上报消息*/
static void platform_headset_notify_cb(void *ctx, E_OPENAT_AUD_HEADSET_NOTIFY_MSG id, uint32 param)
{
	PlatformMsgData rtosmsg;
	//OPENAT_print("platform_headset_notify_cb id = %d,param = %d",id,param);

	rtosmsg.headsetData.msg_id = id;
	rtosmsg.headsetData.param = param;

	platform_rtos_send(MSG_ID_RTOS_HEADSET, &rtosmsg);	

}

void platform_headset_init(BOOL autoControl)
{	
	if(!autoControl)
	{
		OPENAT_handsetSetNotifyCustCB(platform_headset_notify_cb,NULL);
	}
	OPENAT_headsetInit();
}
/*-\new\wj\2020.9.19\lua添加耳机自动检测功能，添加开机和耳机上报消息*/
