#pragma once

#define INTER_CUBE 3
#define INTER_LINEAR 2
#define INTER_COS 1

class GV_MATH_API GV_Noise
{
public:
	GV_Noise(int numSamples = 4);
	~GV_Noise();

public:
	FLOAT fNoise1(int x);
	FLOAT fSmoothNoise1(int x);
	FLOAT fInterpolatedSmoothNoise1(FLOAT x);

	FLOAT fNoise2(int x, int y);
	FLOAT fSmoothNoise2(int x, int y);
	FLOAT fInterpolatedSmoothNoise2(FLOAT x, FLOAT y);

	FLOAT fCos_Interpolate(FLOAT a, FLOAT b, FLOAT Alpha);
	FLOAT fCube_Interpolate(FLOAT v0, FLOAT v1, FLOAT v2, FLOAT v3, FLOAT Alpha);
	FLOAT fLinear_Interpolate(FLOAT a, FLOAT b, FLOAT Alpha);
	FLOAT fRandom(float min, float max);

public:
	FLOAT fNoise1D(FLOAT x);
	FLOAT fNoise2D(FLOAT x, FLOAT y);
	// the output is +2*sqrt(3)---->-2*sqrt(3)
	FLOAT fWhiteNoise();
	// the output is -1--->+1
	// none gauss noise
	FLOAT fRandom();
	BOOL bGenSmoothNoiseMap2D_FFT(FLOAT x, FLOAT y, int w, int h, FLOAT* p_array,
								  int flags);
	BOOL bGenSmoothNoiseMap2D_Perlin(FLOAT x, FLOAT y, int w, int h,
									 FLOAT* p_array, int flags);
	BOOL bGenSmoothNoiseMap2D_Fractal(FLOAT x, FLOAT y, int w, int h,
									  FLOAT* p_array, int flags);

private:
	int m_Interpolation_Type;
	int m_Octaves;
	FLOAT m_Amplitude;
	FLOAT m_Frequency;
	unsigned int m_numSamples;
	FLOAT m_gaussAdd;
	FLOAT m_gaussFactor;

	int Recursive_Mid_Point_QUAD(float* pfdata, int left, int right, int bottom,
								 int top, int w, int h, float factor);
	int Recursive_Mid_Point_Diamond(float* pfdata, int left, int right,
									int bottom, int top, int w, int h,
									float factor);
};

#define ISPOWEROF2(a) (((a) & ((a)-1)) == 0)
