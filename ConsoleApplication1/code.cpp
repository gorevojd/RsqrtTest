#include <xmmintrin.h>
#include <Windows.h>
#include <iostream>

using namespace std;

struct timing_ctx {
	LARGE_INTEGER BeginClock;
	LARGE_INTEGER Freq;
	float OneOverFreq;
	unsigned long long BeginRDTSC;

	char Name[256];
};

void BeginTiming(timing_ctx* Timing, char* Name) {
	strcpy(Timing->Name, Name);
	QueryPerformanceFrequency(&Timing->Freq);
	Timing->OneOverFreq= 1.0f / (float)Timing->Freq.QuadPart;

	Timing->BeginRDTSC = __rdtsc();

	LARGE_INTEGER BeginClockLI;
	QueryPerformanceCounter(&BeginClockLI);
}

void EndTiming(timing_ctx* Timing) {
	LARGE_INTEGER EndClockLI;
	QueryPerformanceCounter(&EndClockLI);
	unsigned long long ElapsedCounter = __rdtsc() - Timing->BeginRDTSC;
	unsigned long long ClocksElapsed = EndClockLI.QuadPart - Timing->BeginClock.QuadPart;
	float DeltaTime = (float)ClocksElapsed * Timing->OneOverFreq;
	
	char Buf[256];
	sprintf(Buf, "%s. Elapsed seconds %.3f, counter clocks %u \n", Timing->Name, DeltaTime, ClocksElapsed);
	printf("%s\n", Buf);

	Timing->Name[0] = 0;
}

int main()
{
	timing_ctx Timing = {};

	int TestCount = 1000000;

	//Testing simple
	BeginTiming(&Timing, "Simple reverse SQRT");
	for (int i = 0; i < TestCount; i++) {
		float Result = 1.0f / sqrtf(23456.0f);
	}
	EndTiming(&Timing);

	//Testing SIMD
	__m128 mmVal = _mm_setr_ps(123.0f, 23467.0f, 123145652.0f, 12315.0f);
	BeginTiming(&Timing, "SIMD reverse SQRT");
	for (int i = 0; i < TestCount; i++) {
		__m128 Result = _mm_rsqrt_ps(mmVal);
	}
	EndTiming(&Timing);

	system("pause");
	return 0;
}