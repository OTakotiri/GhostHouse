#include "XAudio2WaveLoad.h"
#include <crtdbg.h>
#include <stdio.h>
#include <tchar.h>

clsXAudio2WaveLoad::clsXAudio2WaveLoad()
	: m_pFile(nullptr)
	, m_hasGotWaveFormat(false)
	, m_firstSampleOffset(-1)
	, m_dataChunkSize(0)
	, m_dataChunkSamples(0)
{
	ZeroMemory(&m_WaveFormat, sizeof(m_WaveFormat));
}

clsXAudio2WaveLoad::~clsXAudio2WaveLoad()
{
	Close();
}
// オープン
bool clsXAudio2WaveLoad::Open(const char* filename)
{
	if (m_pFile) return false;
	std::string Filename = filename;
	// BGMデータがあるディレクトリパスを入れる.
	std::string BGMPass = "Data\\Sound\\BGM\\" + Filename;
	BGMPass += ".wav";
	LPSTR CastLPSTR = const_cast<char*>(BGMPass.c_str());
	
	// CastLPSTRがエラー値ならSEのファイルの名前の可能性があるので、SEのディレクトリパスで検索.
	if (_tfopen_s(&m_pFile, CastLPSTR, _T("rb")) != 0) {
		std::string SEPass = "Data\\Sound\\SE\\" + Filename;
		SEPass += ".wav";
		LPSTR CastLPSTR = const_cast<char*>(SEPass.c_str());
		if (_tfopen_s(&m_pFile, CastLPSTR, _T("rb")) != 0) {
			// データが存在しない.
			return false;
		}
	}

	Tmp = filename;
	return true;
}


// フォーマット情報を取得.
bool clsXAudio2WaveLoad::GetWaveFormat(const char* filename)
{
	Open(filename);

	// オープン済みか.
	if (!m_pFile) return NULL;

	if (!m_hasGotWaveFormat)
	{
		long offset = 12;

		while (1)
		{
			// チャンク先頭へ移動.
			if (fseek(m_pFile, offset, SEEK_SET) != 0) break;

			// チャンクシグネチャを読み込み.
			char chunkSignature[4] = { 0 };
			std::size_t readChars = 0;
			while (readChars < 4)
			{
				std::size_t ret = fread(chunkSignature + readChars, sizeof(char), 4 - readChars, m_pFile);
				if (ret == 0) break;
				readChars += ret;
			}

			// チャンクサイズを読み込み.
			uint32_t chunkSize = 0;
			if (fread(&chunkSize, sizeof(uint32_t), 1, m_pFile) == 0) break;

			// fmt チャンクが見つかったらフォーマット情報を読み込み.
			if (strncmp(chunkSignature, "fmt ", 4) == 0)
			{
				std::size_t readSize = chunkSize < sizeof(WAVEFORMATEX) ? chunkSize : sizeof(WAVEFORMATEX);
				if (fread(&m_WaveFormat, readSize, 1, m_pFile) == 0) break;

				// PCM のときは一応 cbSize を 0 にしておく.
				if (m_WaveFormat.wFormatTag == WAVE_FORMAT_PCM) m_WaveFormat.cbSize = 0;
				// フォーマット情報取得済み.
				m_hasGotWaveFormat = true;
			}

			// data チャンクが見つかったらオフセットとサイズを記憶.
			if (strncmp(chunkSignature, "data", 4) == 0)
			{
				m_firstSampleOffset = offset + 8;	// シグネチャ 4bytes ＋ サイズ 4bytes.
				m_dataChunkSize = chunkSize;
			}

			// 次のチャンクへ
			offset += (static_cast<long>(chunkSize) + 8);
		}

		if (!m_hasGotWaveFormat) return NULL;	// どっかでエラーが起きてちゃんと拾えなかった.

		// フォーマット情報が取得でき次第 data チャンク内のサンプル数を計算.
		m_dataChunkSamples = m_dataChunkSize / m_WaveFormat.nBlockAlign;	// 必ず割り切れるはず.
	}

	return true;
}
// サンプル数を取得.
size_t clsXAudio2WaveLoad::GetSamples(const char* filename)
{
	// オープン済みか.
	if (!m_pFile) return 0;
	// フォーマット情報を取得していなければここで.
	if (!m_hasGotWaveFormat) GetWaveFormat(filename);

	return m_dataChunkSamples;
}

// Rawデータ読み込み.
size_t clsXAudio2WaveLoad::ReadRaw(const std::size_t start, const std::size_t samples, void * buffer, const char* filename)
{
	// バッファアドレスが不正ではないか.
	if (!buffer) {
		return 0;
		_ASSERT_EXPR(false, L"Rawデータのバッファアドレスが不正値です");
	}
	// オープン済みか.
	if (!m_pFile) return 0;
	// フォーマット情報を取得していなければここで.
	if (!m_hasGotWaveFormat)
	{
		if (!GetWaveFormat(filename)) return 0;
	}
	// 開始位置がオーバーしていないか.
	if (start >= m_dataChunkSamples) return 0;

	// 実際に読み込むサンプル数を計算.
	std::size_t actualSamples = start + samples > m_dataChunkSamples ? m_dataChunkSamples - start : samples;

	// 読み込み開始位置へ移動.
	if (fseek(m_pFile, m_firstSampleOffset + start * m_WaveFormat.nBlockAlign, SEEK_SET) != 0) return 0;
	// 読み込み.
	std::size_t readSamples = 0;
	while (readSamples < actualSamples)
	{
		std::size_t ret = fread(reinterpret_cast<uint8_t *>(buffer) + readSamples * m_WaveFormat.nBlockAlign,
			m_WaveFormat.nBlockAlign,
			actualSamples - readSamples,
			m_pFile);
		if (ret == 0) break;
		readSamples += ret;
	}

	return readSamples;
}

// 正規化済みデータ読み込み.
size_t clsXAudio2WaveLoad::ReadNormalized(const std::size_t start,
	const std::size_t samples, float * left, float * right, const char* filename)
{
	// 少なくとも 1ch ぶんは指定されているか.
	if (!left) {
		_ASSERT_EXPR(false, L"正規化済みデータ読み込みに失敗しました");
	}
	// オープン済みか.
	if (!m_pFile) return 0;
	// フォーマット情報を取得していなければここで.
	if (!m_hasGotWaveFormat)
	{
		if (!GetWaveFormat(filename)) return 0;
	}
	// 開始位置がオーバーしていないか.
	if (start >= m_dataChunkSamples) return 0;

	// 実際に読み込むサンプル数を計算.
	std::size_t actualSamples = start + samples > m_dataChunkSamples ? m_dataChunkSamples - start : samples;

	return 0;
}

// クローズ.
bool clsXAudio2WaveLoad::Close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		m_hasGotWaveFormat = false;
		m_firstSampleOffset = -1;
		m_dataChunkSize = 0;
		m_dataChunkSamples = 0;
	}
	return true;
}

const WAVEFORMATEX& clsXAudio2WaveLoad::GetWaveFmtEx()const
{
	return m_WaveFormat;
}
