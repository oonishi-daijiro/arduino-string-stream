#include <freertos/stream_buffer.h>
#include "bitmap.cpp"
#include "characters.h"

#define MAX_STRING_STREAM_LENGTH 100
#define BUFSIZE (MAX_STRING_STREAM_LENGTH + 1)

constexpr int anodes[5] = {26, 25, 23, 22, 21};
constexpr int cathodes[7] = {4, 13, 14, 16, 17, 32, 18};

void vTaskStreamWriter(void *);
void vTaskStreamReader(void *);

template <typename... PACK>
void __print(PACK... P)
{
   (((Serial.print(P)), Serial.print(" ")), ...);
   Serial.println();
   return;
}

StreamBufferHandle_t hStream;
void setup()
{
   hStream = xStreamBufferCreate(BUFSIZE, 1);
   Serial.begin(9600);

   xTaskCreatePinnedToCore(vTaskStreamWriter, "StreamWriter", 8192, NULL, 1, NULL, 1);
   xTaskCreatePinnedToCore(vTaskStreamReader, "StreamReader", 8192, NULL, 5, NULL, 1);

   pinMode(34, INPUT);

   for (int i = 0; i < 5; i++)
   {
      pinMode(anodes[i], OUTPUT);
      digitalWrite(anodes[i], LOW);
   }
   for (int i = 0; i < 7; i++)
   {
      pinMode(cathodes[i], OUTPUT);
      digitalWrite(cathodes[i], HIGH);
   }
}

void vTaskStreamWriter(void *parm)
{
   for (;;)
   {
      if (Serial.available())
      {
         String line = Serial.readString();
         char buf[BUFSIZE];
         __print("Stream:", line);
         buf[0] = (unsigned char)line.length() - 1;
         for (int i = 1; i <= (line.length() - 1 < BUFSIZE ? line.length() - 1 : BUFSIZE - 1); i++)
         {
            buf[i] = line[i - 1];
         }
         xStreamBufferSend(hStream, buf, sizeof(buf), 0);
      }
   }
}

template <typename T>
void setBMP(Bitmap<T> &bmp, const T *p[], size_t LEN);

template <typename T>
void LEDRow(Bitrow<T> &row);

template <typename T>
void LEDmap(Bitmap<T> &bmp, int duration);

void vTaskStreamReader(void *parm)
{
   Bitmap<unsigned char> bmp(7, MAX_STRING_STREAM_LENGTH);

   for (;;)
   {
      char buf[BUFSIZE];
      xStreamBufferReceive(hStream, buf, sizeof(buf), portMAX_DELAY);
      unsigned char len = buf[0];
      const unsigned char **mapPointerBuf = new const unsigned char *[len];
      int stringLength = 0;
      for (int i = 1; i <= len; i++)
      {
         if (32 <= (unsigned char)buf[i] && (unsigned char)buf[i] <= 126)
         {
            mapPointerBuf[stringLength] = ASCII_CHARACTERS_LED_PATTERNS[(int)buf[i] - 32];
            stringLength++;
         }
      }
      setBMP(bmp, mapPointerBuf, stringLength);
      for (int i = 0; i < stringLength * 8; i++)
      {
         bmp <<= 1;
         LEDmap(bmp, 40);
      }
      delete[] mapPointerBuf;
   }
}

template <typename T>
void setBMP(Bitmap<T> &bmp, const T *p[], size_t LEN)
{
   for (int i = 0; i < bmp.row; i++)
   {
      for (int l = 0; l < LEN; l++)
      {
         bmp[i][l] = p[l][i];
         bmp[i][l] <<= 2;
      }
   }
}

void resetLED()
{
   for (int i = 0; i < 5; i++)
   {
      digitalWrite(anodes[i], LOW);
   }
   for (int i = 0; i < 7; i++)
   {
      digitalWrite(cathodes[i], HIGH);
   }
}

template <typename T>
void LEDRow(Bitrow<T> &row)
{
   for (int i = 0; i < 5; i++)
   {
      T MSB1 = (1 << (sizeof(T) * 8 - 1));
      if ((row[0] & (MSB1 >> i)) == 0)
      {
         digitalWrite(anodes[i], LOW);
      }
      else
      {
         digitalWrite(anodes[i], HIGH);
      }
   }
}

template <typename T>
void LEDmap(Bitmap<T> &bmp, int duration)
{
   int row = 0;
   for (int dur = 0; dur < duration; dur++)
   {
      digitalWrite(cathodes[row], HIGH);
      row = (row + 1) % 7;
      LEDRow(bmp[row]);
      digitalWrite(cathodes[row], LOW);
      vTaskDelay(pdMS_TO_TICKS(3));
   }
   resetLED();
}

void loop(){};
