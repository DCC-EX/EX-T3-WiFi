#ifndef PROGRAM_UI_H
#define PROGRAM_UI_H

#include <UI.h>
#include <DCCExCS.h>
#include <Keypad.h>

class ProgramUI : public UI {
  public:
    enum class Step : uint8_t {
      WRITE_ADDRESS_GET_ADDRESS,

      READ_CV_BYTE_GET_CV,
      WRITE_CV_BYTE_GET_CV,
      WRITE_CV_BYTE_GET_VALUE,

      READ_CV_BIT_GET_CV,
      READ_CV_BIT_GET_BIT,
      WRITE_CV_BIT_GET_CV,
      WRITE_CV_BIT_GET_BIT,
      WRITE_CV_BIT_GET_VALUE,
      
      ACK_LIMIT,
      ACK_MIN,
      ACK_MAX
    };
  private:
    DCCExCS& _dccExCS;
    uint16_t _timeoutHandler;
    uint16_t _writeHandler;
    uint16_t _readHandler;
    Step _step;
    uint16_t _stepData[2];

    void newStep(Step step, const String& title, uint16_t max, uint16_t min);
    void keypadEnter(void* parameter);
    void working();
    void result(const String& label, uint16_t color);
  public:
    ProgramUI(DCCExCS& dccExCS);
    ~ProgramUI();
};

#endif
