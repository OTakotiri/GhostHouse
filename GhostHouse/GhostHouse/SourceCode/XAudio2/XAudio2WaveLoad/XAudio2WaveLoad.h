#ifndef C_XAUDIO2_WAVELOAD_H
#define C_XAUDIO2_WAVELOAD_H

#include "..\XAudio2.h"
#include <string>
class clsXAudio2WaveLoad
{
public:
	clsXAudio2WaveLoad();
	~clsXAudio2WaveLoad();

	// WAVE フォーマットを取得.
	const WAVEFORMATEX&     GetWaveFmtEx()const;
	// WAVEデータオープン関数.
	bool Open(const char * filename);
	// フォーマット情報を取得.
	bool GetWaveFormat(const char* filename);
	// サンプル数を取得.
	size_t GetSamples(const char* filename);
	// Rawデータ読み込み.
	size_t ReadRaw(const std::size_t start, const std::size_t samples, void * buffer, const char* filename);
	// 正規化済みデータ読み込み.
	size_t ReadNormalized(const std::size_t start, const std::size_t samples, float * left, float * right, const char* filename);

	const char* GetFileName() {
		return Tmp;
	}
	bool Close();
private:
	const char* Tmp;

	// ファイルハンドル.
	FILE* m_pFile;
	// フォーマット情報を取得済みか.
	bool m_hasGotWaveFormat;
	// フォーマット情報.
	WAVEFORMATEX m_WaveFormat;
	// data チャンク内先頭サンプルへのオフセット.
	long m_firstSampleOffset;
	// data チャンクサイズ.
	size_t m_dataChunkSize;
	// data チャンク内サンプル数.
	size_t m_dataChunkSamples;
};

#endif //#ifndef C_XAUDIO2_WAVELOAD_H