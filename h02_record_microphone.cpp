#include "main.h"


#ifdef H02_RECORD_MICROPHONE



#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"
#include "webrtc/base/win32socketserver.h"

#include "webrtc\voice_engine\voe_file_impl.h"
#include "webrtc\voice_engine\include\voe_base.h"
#include "webrtc/modules/audio_device/include/audio_device.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <assert.h>
#include "webrtc/modules/audio_device/include/audio_device.h"
#include "webrtc/common_audio/resampler/include/resampler.h"
#include "webrtc/modules/audio_processing/aec/include/echo_cancellation.h"
#include "webrtc/common_audio/vad/include/webrtc_vad.h"

#include "dbgtool.h"
#include "string_useful.h"

using namespace webrtc;
VoiceEngine* g_voe = NULL;
VoEBase* g_base = NULL;
VoEFile* g_file = NULL;
int g_ch = -1;

HANDLE g_hEvQuit = NULL;


void Begin_RecordMicrophone();
void End_RecordMicrophone();

/////////////////////////////////////////////////////////////////////////
// 录制microphone输入的声音
void Begin_RecordMicrophone()
{
  int iRet = -1;
    
  g_voe = VoiceEngine::Create();
  g_base = VoEBase::GetInterface(g_voe);
  g_file = VoEFile::GetInterface(g_voe);

  g_base->Init();
  //g_ch = g_base->CreateChannel();
  //g_base->StartPlayout(g_ch);

  g_hEvQuit = CreateEvent(NULL, FALSE, FALSE, NULL);
  
  // 录制输入的microphone的声音到文件
  //iRet = g_file->StartPlayingFileLocally(g_ch, "E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16_from_microphone.wav", true);
  iRet = g_file->StartRecordingMicrophone("E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16_from_microphone.wav");
  //g_base->StartRecord();
  

  while (TRUE) {
    DWORD dwRet = ::WaitForSingleObject(g_hEvQuit, 500);
    if (dwRet == WAIT_OBJECT_0) {
      End_RecordMicrophone();
      break;
    }
  }
}

void End_RecordMicrophone()
{
  //g_file->StopRecordingMicrophone();
  g_base->Terminate();
  g_base->Release();
  g_file->Release();

  VoiceEngine::Delete(g_voe);
}

DWORD WINAPI ThreadFunc(LPVOID lpParameter) {

  Begin_RecordMicrophone();

  return 0;
}

int main()
{
  // 初始化SSL
  rtc::InitializeSSL();

  DWORD IDThread;
  HANDLE hThread;
  DWORD ExitCode;

  hThread = CreateThread(NULL,
    0,
    (LPTHREAD_START_ROUTINE)ThreadFunc,
    NULL,
    0,
    &IDThread);
  if (hThread == NULL) {
    return -1;
  }

  printf("Input 'Q' to stop recording!!!");
  char ch;
  while (ch = getch()) {
    if (ch == 'Q') {
      if (g_hEvQuit) {
        ::SetEvent(g_hEvQuit);
        if (hThread) {
          ::WaitForSingleObject(hThread, INFINITE);
          CloseHandle(hThread);
          hThread = NULL;
        }

        CloseHandle(g_hEvQuit);
        g_hEvQuit = NULL;
      }
      break;
    }
  }

  rtc::CleanupSSL();

  return 0;
}

#endif // H02_RECORD_MICROPHONE

