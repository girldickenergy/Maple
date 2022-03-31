#include "lzma_helper.hpp"

namespace lh {

	bool CRangeDecoder::Init()
	{
		Corrupted = false;
		Range = 0xFFFFFFFF;
		Code = 0;

		Byte b = InStream->ReadByte();

		for (int i = 0; i < 4; i++)
			Code = (Code << 8) | InStream->ReadByte();

		if (b != 0 || Code == Range)
			Corrupted = true;
		return b == 0;
	}

#define kTopValue ((UInt32)1 << 24)

	COutWindow::COutWindow(basic_output_stream* out_stream) : Buf(NULL), OutStream{ out_stream } {}


	COutWindow::~COutWindow()
	{
		delete[]Buf;
	}

	void COutWindow::Create(UInt32 dictSize)
	{
		Buf = new Byte[dictSize];
		Pos = 0;
		Size = dictSize;
		IsFull = false;
		TotalPos = 0;
	}

	void COutWindow::PutByte(Byte b)
	{
		TotalPos++;
		Buf[Pos++] = b;
		if (Pos == Size) {
			Pos = 0;
			IsFull = true;
		}
		OutStream->WriteByte(b);
	}

	Byte COutWindow::GetByte(UInt32 dist) const
	{
		return Buf[dist <= Pos ? Pos - dist : Size - dist + Pos];
	}

	void COutWindow::CopyMatch(UInt32 dist, unsigned len)
	{
		for (; len > 0; len--)
			PutByte(GetByte(dist));
	}

	bool COutWindow::CheckDistance(UInt32 dist) const
	{
		return dist <= Pos || IsFull;
	}

	bool COutWindow::IsEmpty() const
	{
		return Pos == 0 && !IsFull;
	}

	void CRangeDecoder::Normalize()
	{
		if (Range < kTopValue)
		{
			Range <<= 8;
			Code = (Code << 8) | InStream->ReadByte();
		}
	}

	UInt32 CRangeDecoder::DecodeDirectBits(unsigned numBits)
	{
		UInt32 res = 0;
		do
		{
			Range >>= 1;
			Code -= Range;
			UInt32 t = 0 - ((UInt32)Code >> 31);
			Code += Range & t;

			if (Code == Range)
				Corrupted = true;

			Normalize();
			res <<= 1;
			res += t + 1;
		} while (--numBits);
		return res;
	}

	unsigned CRangeDecoder::DecodeBit(CProb *prob)
	{
		unsigned v = *prob;
		UInt32 bound = (Range >> kNumBitModelTotalBits) * v;
		unsigned symbol;
		if (Code < bound)
		{
			v += ((1 << kNumBitModelTotalBits) - v) >> kNumMoveBits;
			Range = bound;
			symbol = 0;
		}
		else
		{
			v -= v >> kNumMoveBits;
			Code -= bound;
			Range -= bound;
			symbol = 1;
		}
		*prob = (CProb)v;
		Normalize();
		return symbol;
	}


	unsigned BitTreeReverseDecode(CProb *probs, unsigned numBits, CRangeDecoder *rc)
	{
		unsigned m = 1;
		unsigned symbol = 0;
		for (unsigned i = 0; i < numBits; i++)
		{
			unsigned bit = rc->DecodeBit(&probs[m]);
			m <<= 1;
			m += bit;
			symbol |= (bit << i);
		}
		return symbol;
	}





	unsigned UpdateState_Literal(unsigned state)
	{
		if (state < 4) return 0;
		else if (state < 10) return state - 3;
		else return state - 6;
	}
	unsigned UpdateState_Match(unsigned state) { return state < 7 ? 7 : 10; }
	unsigned UpdateState_Rep(unsigned state) { return state < 7 ? 8 : 11; }
	unsigned UpdateState_ShortRep(unsigned state) { return state < 7 ? 9 : 11; }


	void CLenDecoder::Init()
	{
		Choice = PROB_INIT_VAL;
		Choice2 = PROB_INIT_VAL;
		HighCoder.Init();
		for (unsigned i = 0; i < (1 << kNumPosBitsMax); i++) {
			LowCoder[i].Init();
			MidCoder[i].Init();
		}
	}

	unsigned CLenDecoder::Decode(CRangeDecoder* rc, unsigned posState)
	{
		if (rc->DecodeBit(&Choice) == 0)
			return LowCoder[posState].Decode(rc);
		if (rc->DecodeBit(&Choice2) == 0)
			return 8 + MidCoder[posState].Decode(rc);
		return 16 + HighCoder.Decode(rc);
	}

	void CLzmaDecoder::DecodeProperties(const Byte* properties)
	{
		unsigned d = properties[0];
		if (d >= (9 * 5 * 5))
			throw "Incorrect LZMA properties";
		lc = d % 9;
		d /= 9;
		pb = d / 5;
		lp = d % 5;
		dictSizeInProperties = 0;
		for (int i = 0; i < 4; i++)
			dictSizeInProperties |= (UInt32)properties[i + 1] << (8 * i);
		dictSize = dictSizeInProperties;
		if (dictSize < LZMA_DIC_MIN)
			dictSize = LZMA_DIC_MIN;
	}

	CLzmaDecoder::CLzmaDecoder(basic_output_stream* out_stream) : OutWindow{ out_stream }, LitProbs(NULL) {}

	CLzmaDecoder::~CLzmaDecoder()
	{
		delete[]LitProbs;
	}

	void CLzmaDecoder::Create()
	{
		OutWindow.Create(dictSize);
		CreateLiterals();
	}

	void CLzmaDecoder::CreateLiterals()
	{
		LitProbs = new CProb[(UInt32)0x300 << (lc + lp)];
	}

	void CLzmaDecoder::InitLiterals()
	{
		UInt32 num = (UInt32)0x300 << (lc + lp);
		for (UInt32 i = 0; i < num; i++)
			LitProbs[i] = PROB_INIT_VAL;
	}

	void CLzmaDecoder::DecodeLiteral(unsigned state, UInt32 rep0)
	{
		unsigned prevByte = 0;
		if (!OutWindow.IsEmpty())
			prevByte = OutWindow.GetByte(1);

		unsigned symbol = 1;
		unsigned litState = ((OutWindow.TotalPos & ((1 << lp) - 1)) << lc) + (prevByte >> (8 - lc));
		CProb* probs = &LitProbs[(UInt32)0x300 * litState];

		if (state >= 7) {
			unsigned matchByte = OutWindow.GetByte(rep0 + 1);
			do {
				unsigned matchBit = (matchByte >> 7) & 1;
				matchByte <<= 1;
				unsigned bit = RangeDec.DecodeBit(&probs[((1 + matchBit) << 8) + symbol]);
				symbol = (symbol << 1) | bit;
				if (matchBit != bit)
					break;
			} while (symbol < 0x100);
		}
		while (symbol < 0x100)
			symbol = (symbol << 1) | RangeDec.DecodeBit(&probs[symbol]);
		OutWindow.PutByte((Byte)(symbol - 0x100));
	}

	void CLzmaDecoder::InitDist()
	{
		for (unsigned i = 0; i < kNumLenToPosStates; i++)
			PosSlotDecoder[i].Init();
		AlignDecoder.Init();
		INIT_PROBS(PosDecoders);
	}

	unsigned CLzmaDecoder::DecodeDistance(unsigned len)
	{
		unsigned lenState = len;
		if (lenState > kNumLenToPosStates - 1)
			lenState = kNumLenToPosStates - 1;

		unsigned posSlot = PosSlotDecoder[lenState].Decode(&RangeDec);
		if (posSlot < 4)
			return posSlot;

		unsigned numDirectBits = (unsigned)((posSlot >> 1) - 1);
		UInt32 dist = ((2 | (posSlot & 1)) << numDirectBits);
		if (posSlot < kEndPosModelIndex)
			dist += BitTreeReverseDecode(PosDecoders + dist - posSlot, numDirectBits, &RangeDec);
		else {
			dist += RangeDec.DecodeDirectBits(numDirectBits - kNumAlignBits) << kNumAlignBits;
			dist += AlignDecoder.ReverseDecode(&RangeDec);
		}
		return dist;
	}

	void CLzmaDecoder::Init()
	{
		InitLiterals();
		InitDist();

		INIT_PROBS(IsMatch);
		INIT_PROBS(IsRep);
		INIT_PROBS(IsRepG0);
		INIT_PROBS(IsRepG1);
		INIT_PROBS(IsRepG2);
		INIT_PROBS(IsRep0Long);

		LenDecoder.Init();
		RepLenDecoder.Init();
	}

	std::vector<Byte> lzma_decompress(const std::vector<Byte>& data)
	{
		vector_input_stream input{ data };
		vector_output_stream output{};

		CLzmaDecoder lzmaDecoder{ &output };

		Byte header[13];
		int i;
		for (i = 0; i < 13; i++)
			header[i] = input.ReadByte();

		lzmaDecoder.DecodeProperties(header);

		UInt64 unpackSize = 0;
		bool unpackSizeDefined = false;
		for (i = 0; i < 8; i++) {
			Byte b = header[5 + i];
			if (b != 0xFF)
				unpackSizeDefined = true;
			unpackSize |= static_cast<UInt64>(b) << (8 * i);
		}

		lzmaDecoder.markerIsMandatory = !unpackSizeDefined;
		lzmaDecoder.RangeDec.InStream = &input;

		lzmaDecoder.Create();

		int res = lzmaDecoder.Decode(unpackSizeDefined, unpackSize);

		return output.get_data();
	}

	int CLzmaDecoder::Decode(bool unpackSizeDefined, UInt64 unpackSize)
	{
		if (!RangeDec.Init())
			return LZMA_RES_ERROR;

		Init();

		UInt32 rep0 = 0, rep1 = 0, rep2 = 0, rep3 = 0;
		unsigned state = 0;

		for (;;)
		{
			if (unpackSizeDefined && unpackSize == 0 && !markerIsMandatory)
				if (RangeDec.IsFinishedOK())
					return LZMA_RES_FINISHED_WITHOUT_MARKER;

			unsigned posState = OutWindow.TotalPos & ((1 << pb) - 1);

			if (RangeDec.DecodeBit(&IsMatch[(state << kNumPosBitsMax) + posState]) == 0)
			{
				if (unpackSizeDefined && unpackSize == 0)
					return LZMA_RES_ERROR;
				DecodeLiteral(state, rep0);
				state = UpdateState_Literal(state);
				unpackSize--;
				continue;
			}

			unsigned len;

			if (RangeDec.DecodeBit(&IsRep[state]) != 0)
			{
				if (unpackSizeDefined && unpackSize == 0)
					return LZMA_RES_ERROR;
				if (OutWindow.IsEmpty())
					return LZMA_RES_ERROR;
				if (RangeDec.DecodeBit(&IsRepG0[state]) == 0)
				{
					if (RangeDec.DecodeBit(&IsRep0Long[(state << kNumPosBitsMax) + posState]) == 0)
					{
						state = UpdateState_ShortRep(state);
						OutWindow.PutByte(OutWindow.GetByte(rep0 + 1));
						unpackSize--;
						continue;
					}
				}
				else
				{
					UInt32 dist;
					if (RangeDec.DecodeBit(&IsRepG1[state]) == 0)
						dist = rep1;
					else
					{
						if (RangeDec.DecodeBit(&IsRepG2[state]) == 0)
							dist = rep2;
						else
						{
							dist = rep3;
							rep3 = rep2;
						}
						rep2 = rep1;
					}
					rep1 = rep0;
					rep0 = dist;
				}
				len = RepLenDecoder.Decode(&RangeDec, posState);
				state = UpdateState_Rep(state);
			}
			else
			{
				rep3 = rep2;
				rep2 = rep1;
				rep1 = rep0;
				len = LenDecoder.Decode(&RangeDec, posState);
				state = UpdateState_Match(state);
				rep0 = DecodeDistance(len);
				if (rep0 == 0xFFFFFFFF)
					return RangeDec.IsFinishedOK() ?
					LZMA_RES_FINISHED_WITH_MARKER :
					LZMA_RES_ERROR;

				if (unpackSizeDefined && unpackSize == 0)
					return LZMA_RES_ERROR;
				if (rep0 >= dictSize || !OutWindow.CheckDistance(rep0))
					return LZMA_RES_ERROR;
			}
			len += kMatchMinLen;
			bool isError = false;
			if (unpackSizeDefined && unpackSize < len)
			{
				len = (unsigned)unpackSize;
				isError = true;
			}
			OutWindow.CopyMatch(rep0 + 1, len);
			unpackSize -= len;
			if (isError)
				return LZMA_RES_ERROR;
		}
	}
}

