#include <iostream>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

void notify() {
	PlaySoundA("D:\\Code_projects\\BallKnowledge\\assets\\sound.wav", NULL, SND_FILENAME | SND_ASYNC);
}