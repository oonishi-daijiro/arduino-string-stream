#include <cstddef>

template <typename T>
constexpr T MSB_1 = (T)(1 << (sizeof(T) * 8 - 1));

template <typename T>
class Bitrow
{
public:
   Bitrow(size_t col) : col(col)
   {
      p = new T[col];
      for (int i = 0; i < col; i++)
      {
         p[i] = (T)0;
      }
   }
   ~Bitrow()
   {
      delete[] p;
   }
   Bitrow &operator>>=(int shiftcout)
   {
      for (int i = 0; i < shiftcout; i++)
      {
         bitShiftRight();
      }
      return *this;
   };

   Bitrow &operator<<=(int shiftcount)
   {
      for (int i = 0; i < shiftcount; i++)
      {
         bitshiftLeft();
      }
      return *this;
   };

   T &operator[](int index)
   {
      return p[index];
   }

   const size_t col;

private:
   T *p;

   void bitShiftRight()
   {
      for (int i = col - 1; i >= 0; i--)
      {
         p[i] >>= 1;
         if (i >= 1)
         {
            p[i] |= ((p[i - 1] & 1) == 0) ? 0 : (MSB_1<T>);
         }
      }
   }
   void bitshiftLeft()
   {
      for (int i = 0; i < col; i++)
      {
         p[i] <<= 1;
         if (i < col - 1)
         {
            p[i] |= ((p[i + 1] & MSB_1<T>) == 0) ? 0 : 1;
         }
      }
   }
};

template <typename T>
class Bitmap
{
public:
   Bitmap(size_t row, size_t col) : row(row)
   {
      p = new Bitrow<T> *[row];

      for (int i = 0; i < row; i++)
      {
         p[i] = new Bitrow<T>(col);
      }
   }

   ~Bitmap()
   {
      for (int i = 0; i < row; i++)
      {
         delete p[i];
      }
      delete[] p;
   }
   Bitmap &operator>>=(int shiftcount)
   {
      for (int i = 0; i < row; i++)
      {
         (*p[i]) >>= shiftcount;
      }
      return *this;
   }
   Bitmap &operator<<=(int shiftcount)
   {
      for (int i = 0; i < row; i++)
      {
         (*p[i]) <<= shiftcount;
      }
      return *this;
   }
   Bitrow<T> &operator[](size_t index)
   {
      return *p[index];
   }

   const size_t row;

private:
   Bitrow<T> **p;
};
