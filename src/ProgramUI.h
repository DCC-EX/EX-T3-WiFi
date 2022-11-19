#ifndef PROGRAM_UI_H
#define PROGRAM_UI_H

#include <UI.h>
#include <DCCExCS.h>
#include <Keypad.h>

class ProgramUI : public UI {
  public:
    enum class Step : uint8_t {
      WRITE_ADDRESS_GET_ADDRESS,
      WRITE_CV_BYTE_GET_CV,
      WRITE_CV_BYTE_GET_VALUE,
      WRITE_CV_BIT_GET_CV,
      WRITE_CV_BIT_GET_BIT,
      WRITE_CV_BIT_GET_VALUE,
      READ_CV_BYTE_GET_CV,

      ACK_LIMIT,
      ACK_MIN,
      ACK_MAX
    };
  private:
    DCCExCS *_dccExCS;
    uint16_t _timeoutHandler;
    uint16_t _writeHandler;
    uint16_t _readHandler;
    Step _step;
    uint16_t _stepData[2];
    std::unique_ptr<Keypad> _keypad;

    void buildUI();
    void clearUI();
    void newStep(Step step, const char *title, uint16_t max, uint16_t min);
    void keyPadEnter();
    void working();
    void result(const char *label, uint16_t color);
  public:
    ProgramUI(TFT_eSPI *tft, Tasks *tasks, DCCExCS *dccExCS);
    ~ProgramUI();
    
    void rotated();
    
    void touch(uint8_t count, GTPoint* points);
    void release(uint8_t count, GTPoint* points);
};

#endif
