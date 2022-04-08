#pragma once
#include <stdio.h>
#include <vector>
#include <stdexcept>


/* LzmaSpec.cpp -- LZMA Reference Decoder
2015-06-14 : Igor Pavlov : Public domain */

// This code implements LZMA file decoding according to LZMA specification.
// This code is not optimized for speed.

#ifdef _MSC_VER
#pragma warning(disable : 4710) // function not inlined
#pragma warning(disable : 4996) // This function or variable may be unsafe
#endif

namespace lh {

	using Byte = unsigned char;
	using UInt16 = unsigned short;

#ifdef _LZMA_UINT32_IS_ULONG
	using UInt32 = unsigned long;
#else
	using UInt32 = unsigned int;
#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
	using UInt64 = unsigned __int64;
#else
	using UInt64 = unsigned long long int;
#endif

#define kNumBitModelTotalBits 11
#define kNumMoveBits 5

#define PROB_INIT_VAL ((1 << kNumBitModelTotalBits) / 2)

#define INIT_PROBS(p) \
 { for (unsigned i = 0; i < sizeof(p) / sizeof(p[0]); i++) p[i] = PROB_INIT_VAL; }



	typedef UInt16 CProb;

#define kNumPosBitsMax 4

#define kNumStates 12
#define kNumLenToPosStates 4
#define kNumAlignBits 4
#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))
#define kMatchMinLen 2


#define LZMA_DIC_MIN (1 << 12)

#define LZMA_RES_ERROR                   0
#define LZMA_RES_FINISHED_WITH_MARKER    1
#define LZMA_RES_FINISHED_WITHOUT_MARKER 2

	struct basic_input_stream {
		virtual ~basic_input_stream() {}

		virtual  Byte ReadByte() = 0;
	};

	struct CInputStream : public basic_input_stream
	{
		explicit CInputStream(FILE* file) : File{ file } { Processed = 0; }

		FILE *File;
		UInt64 Processed;

		Byte ReadByte() override
		{
			int c = getc(File);
			if (c < 0)
				throw "Unexpected end of file";
			Processed++;
			return (Byte)c;
		}
	};

	struct vector_input_stream : public basic_input_stream {
		explicit vector_input_stream(const std::vector<Byte>& p_data)
			: data{ p_data }, it{ data.cbegin() } {}

		const std::vector<Byte>& data;
		std::vector<Byte>::const_iterator it;

		Byte ReadByte() override
		{
			if (data.cend() == it)
				throw std::runtime_error{ "Reached end of vector!" };

			Byte byte = *it;
			++it;
			return byte;
		}
	};


	struct basic_output_stream {
		virtual ~basic_output_stream() {}

		virtual void WriteByte(Byte b) = 0;
	};

	struct vector_output_stream : public basic_output_stream {

		std::vector<Byte> data;

		void WriteByte(Byte b) override
		{
			data.push_back(b);
		}
		std::vector<Byte> get_data() { return data; }
	};


	struct COutStream : public basic_output_stream
	{
		FILE *File;
		UInt64 Processed;

		void Init() { Processed = 0; }

		void WriteByte(Byte b)
		{
			if (putc(b, File) == EOF)
				throw "File writing error";
			Processed++;
		}
	};

	class COutWindow
	{
		Byte *Buf;
		UInt32 Pos;
		UInt32 Size;
		bool IsFull;

	public:
		unsigned TotalPos;
		basic_output_stream* OutStream;

		COutWindow(basic_output_stream* out_stream);

		~COutWindow();

		void Create(UInt32 dictSize);

		void PutByte(Byte b);

		Byte GetByte(UInt32 dist) const;

		void CopyMatch(UInt32 dist, unsigned len);

		bool CheckDistance(UInt32 dist) const;

		bool IsEmpty() const;
	};

	class CRangeDecoder
	{
		UInt32 Range;
		UInt32 Code;

		void Normalize();

	public:

		basic_input_stream *InStream;
		bool Corrupted;

		bool Init();
		bool IsFinishedOK() const { return Code == 0; }

		UInt32 DecodeDirectBits(unsigned numBits);
		unsigned DecodeBit(CProb *prob);
	};

	template <unsigned NumBits>
	class CBitTreeDecoder
	{
		CProb Probs[(unsigned)1 << NumBits];

	public:

		void Init()
		{
			INIT_PROBS(Probs);
		}

		unsigned Decode(CRangeDecoder *rc)
		{
			unsigned m = 1;
			for (unsigned i = 0; i < NumBits; i++)
				m = (m << 1) + rc->DecodeBit(&Probs[m]);
			return m - ((unsigned)1 << NumBits);
		}

		unsigned ReverseDecode(CRangeDecoder *rc)
		{
			return BitTreeReverseDecode(Probs, NumBits, rc);
		}
	};


	class CLenDecoder
	{
		CProb Choice;
		CProb Choice2;
		CBitTreeDecoder<3> LowCoder[1 << kNumPosBitsMax];
		CBitTreeDecoder<3> MidCoder[1 << kNumPosBitsMax];
		CBitTreeDecoder<8> HighCoder;

	public:

		void Init();

		unsigned Decode(CRangeDecoder* rc, unsigned posState);
	};

	class CLzmaDecoder
	{
	public:
		CRangeDecoder RangeDec;
		COutWindow OutWindow;

		bool markerIsMandatory;
		unsigned lc, pb, lp;
		UInt32 dictSize;
		UInt32 dictSizeInProperties;

		void DecodeProperties(const Byte* properties);

		CLzmaDecoder(basic_output_stream* out_stream);

		~CLzmaDecoder();

		void Create();

		int Decode(bool unpackSizeDefined, UInt64 unpackSize);

	private:

		CProb *LitProbs;

		void CreateLiterals();

		void InitLiterals();

		void DecodeLiteral(unsigned state, UInt32 rep0);

		CBitTreeDecoder<6> PosSlotDecoder[kNumLenToPosStates];
		CBitTreeDecoder<kNumAlignBits> AlignDecoder;
		CProb PosDecoders[1 + kNumFullDistances - kEndPosModelIndex];

		void InitDist();

		unsigned DecodeDistance(unsigned len);

		CProb IsMatch[kNumStates << kNumPosBitsMax];
		CProb IsRep[kNumStates];
		CProb IsRepG0[kNumStates];
		CProb IsRepG1[kNumStates];
		CProb IsRepG2[kNumStates];
		CProb IsRep0Long[kNumStates << kNumPosBitsMax];

		CLenDecoder LenDecoder;
		CLenDecoder RepLenDecoder;

		void Init();
	};

	std::vector<Byte> lzma_decompress(const std::vector<Byte>& data);
}

