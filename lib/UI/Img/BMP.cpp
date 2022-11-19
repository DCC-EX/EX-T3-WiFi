#include <Img/BMP.h>

BMP::BMP(fs::File bmp) : Img(bmp) { }

BMP::BMP(fs::File bmp, TFT_eSPI *tft, int16_t x, int16_t y) : BMP(bmp) {
  draw(tft, x, y);
}

uint16_t BMP::read16() {
  uint16_t result;
  _img.read((uint8_t*)&result, sizeof(result));
  return result;
}

uint32_t BMP::read32() {
  uint32_t result;
  _img.read((uint8_t*)&result, sizeof(result));
  return result;
}

bool BMP::dimensions(uint32_t &w, uint32_t &h) {
  if (read16() == 0x4D42) {
    _img.seek(18);
    w = read32();
    h = read32();
    _img.seek(0);
    return true;
  }
  return false;
}

bool BMP::draw(TFT_eSPI *tft, int16_t x, int16_t y) {
  if ((x >= tft->width()) || (y >= tft->height())) {
    return false;
  }

  uint32_t seekOffset;
  uint16_t w, h, row;
  uint8_t  r, g, b;

  if (read16() == 0x4D42) {
    read32();
    read32();
    seekOffset = read32();
    read32();
    w = read32();
    h = read32();

    if (read16() == 1 && read16() == 24 && read32() == 0) {
      y += h - 1;

      bool oldSwapBytes = tft->getSwapBytes();
      tft->setSwapBytes(true);
      _img.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        _img.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = reinterpret_cast<uint16_t*>(lineBuffer);
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++) {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        tft->pushImage(x, y--, w, 1, reinterpret_cast<uint16_t*>(lineBuffer));
      }
      tft->setSwapBytes(oldSwapBytes);
    } else {
      return false;
    }
  }

  return true;
}
