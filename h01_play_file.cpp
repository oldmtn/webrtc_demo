#include "main.h"


#ifdef H01_PLAY_FILE



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
//#include "log.cpp"

#include "dbgtool.h"
#include "string_useful.h"




// This sub-API supports the following functionalities:
//
//  - File playback.
//  - File recording.
//  - File conversion.
//
// Usage example, omitting error checking:
//
//  using namespace webrtc;
//  VoiceEngine* voe = VoiceEngine::Create();
//  VoEBase* base = VoEBase::GetInterface(voe);
//  VoEFile* file  = VoEFile::GetInterface(voe);
//  base->Init();
//  int ch = base->CreateChannel();
//  ...
//  base->StartPlayout(ch);
//  file->StartPlayingFileAsMicrophone(ch, "data_file_16kHz.pcm", true);
//  ...
//  file->StopPlayingFileAsMicrophone(ch);
//  base->StopPlayout(ch);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  file->Release();
//  VoiceEngine::Delete(voe);

///////////////////////////////////////////////////
using namespace webrtc;
VoiceEngine* g_voe = NULL;
VoEBase* g_base = NULL;
VoEFile* g_file = NULL;
int g_ch = -1;

HANDLE g_hEvQuit = NULL;
void Begin_PlayFile();
void End_PlayFile();

// 录制microphone输入的声音
void Begin_PlayFile()
{
  int iRet = -1;

  using namespace webrtc;
  g_voe = VoiceEngine::Create();
  g_base = VoEBase::GetInterface(g_voe);
  g_file = VoEFile::GetInterface(g_voe);
  g_base->Init();
  g_ch = g_base->CreateChannel();

  // 播放输入文件audio_long16.pcm并将其录入到audio_long16_out.pcm中
  //iRet = file->StartPlayingFileLocally(ch, "E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16.pcm", true);
  //iRet = file->StartRecordingPlayout(ch, "E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16_out.pcm");

  //iRet = file->StartRecordingMicrophone("E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16_from_microphone.pcm");
  iRet = g_file->StartPlayingFileLocally(g_ch, "E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_tiny16.wav", false);
  //iRet = g_file->StartPlayingFileAsMicrophone(g_ch,
  //  "E:\\webrtc_compile\\webrtc_windows\\src\\talk\\examples\\hh_sample\\audio_long16.pcm",
  //  true,
  //  true);
  
  // 开始播放
  g_base->StartPlayout(g_ch);

  while (TRUE) {
    DWORD dwRet = ::WaitForSingleObject(g_hEvQuit, INFINITE);
    if (dwRet == WAIT_OBJECT_0) {
      End_PlayFile();
      break;
    }
  }
}

void End_PlayFile()
{
  g_base->StopPlayout(g_ch); 
  g_file->StopPlayingFileLocally(g_ch);
  //g_file->StopPlayingFileAsMicrophone(g_ch);
  
  g_base->DeleteChannel(g_ch);
  g_base->Terminate();
  g_base->Release();
  g_file->Release();

  VoiceEngine::Delete(g_voe);
}

////////////////////////////////////////////////////////////////////
DWORD WINAPI ThreadFunc(LPVOID lpParameter) {

  Begin_PlayFile();

  return 0;
}

int main()
{
  // 初始化SSL
  rtc::InitializeSSL();

  DWORD IDThread;
  HANDLE hThread;

  hThread = CreateThread(NULL,
    0,
    (LPTHREAD_START_ROUTINE)ThreadFunc,
    NULL,
    0,
    &IDThread);
  if (hThread == NULL) {
    return -1;
  }

  printf("Input 'Q' to stop play!!!");
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





#endif // H01_PLAY_FILE

