// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

TBitField::TBitField(int len)
{
    if (len < 1) throw "Wrong length";
    BitLen = len;
    MemLen = (BitLen + sizeof(TELEM) * 8 - 1) / (sizeof(TELEM) * 8);         // расчёт количества элементов для хранения битового поля
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++)
        pMem[i] = 0;
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++)
        pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
    BitLen = MemLen = 0;
    delete []pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if (n < 0 || n >= BitLen) throw "Wrong size";
    return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    if (n < 0 || n >= BitLen) throw "Wrong size";
    return TELEM(1 << (sizeof(TELEM) * 8 - (n % (sizeof(TELEM) * 8)) - 1));
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen) throw "Wrong size";
    TELEM MemInd = GetMemIndex(n);
    pMem[MemInd] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen) throw "Wrong size";
    TELEM MemInd = GetMemIndex(n);
    pMem[MemInd] = pMem[MemInd] & ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    TELEM MemInd = GetMemIndex(n);
    return (pMem[MemInd] & GetMemMask(n)) >> sizeof(TELEM) * 8 - (n % (sizeof(TELEM) * 8)) - 1;
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    delete[] pMem;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++)
        pMem[i] = bf.pMem[i];
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    if (BitLen != bf.BitLen)
        return 0;
    for (int i = 0; i < MemLen; i++)
        if (pMem[i] != bf.pMem[i])
            return 0;
    return 1;
  
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
  return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    TELEM MaxMemLen, MinMemLen;
    if (MemLen < bf.MemLen)
    {
        MaxMemLen = bf.MemLen;
        MinMemLen = MemLen;
    }
    else if (MemLen > bf.MemLen)
    {
        MaxMemLen = MemLen;
        MinMemLen = bf.MemLen;
    }
    else
        MaxMemLen = MinMemLen = MemLen;
    TBitField res(max(BitLen, bf.BitLen));
    for (int i = 0; i < MaxMemLen; i++)
        if (i < MinMemLen)
            res.pMem[i] = pMem[i] | bf.pMem[i];
        else {        // учёт случая если поле MemLen операндов различное
            if (MemLen > bf.MemLen) res.pMem[i] = pMem[i];
            else res.pMem[i] = bf.pMem[i];

        }
    return res;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    TELEM MaxMemLen = max(MemLen, bf.MemLen);
    TELEM MinMemLen = min(MemLen, bf.MemLen);
    TBitField res(max(BitLen, bf.BitLen));
    for (int i = 0; i < MaxMemLen; i++)
        if (i < MinMemLen)
            res.pMem[i] = pMem[i] & bf.pMem[i];
        else res.pMem[i] = 0;      // учёт случая, если поле MemLen у операндов различное
    return res;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField res(BitLen);
    for (int i = 0; i < MemLen - 1; i++)
        res.pMem[i] = ~pMem[i];
    res.pMem[MemLen - 1] = ~(pMem[MemLen - 1] | ((GetMemMask(BitLen - 1)) - 1));
    return res;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    char ch;
    for (int i = 0; i < bf.BitLen; i++)
    {
        istr >> ch;
        if (ch == '1')
            bf.SetBit(i);
        else if (ch == '0')
            bf.ClrBit(i);
        else
            throw invalid_argument("Invalid character in bit field input");
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++)
    {
        ostr << bf.GetBit(i);
    }
    return ostr;
}
