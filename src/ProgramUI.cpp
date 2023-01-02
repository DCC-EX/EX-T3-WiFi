#include <ProgramUI.h>
#include <Elements/Button.h>
#include <MessageBox.h>

ProgramUI::ProgramUI(DCCExCS& dccExCS) : _dccExCS(dccExCS) {
  _elements.reserve(11);

  _timeoutHandler = _dccExCS.addEventListener(DCCExCS::Event::TIMEOUT, [this](void*) {
    result("Timeout", TFT_RED);
  });
  _writeHandler = _dccExCS.addEventListener(DCCExCS::Event::PROGRAM_WRITE, [this](void* parameter) {
    if (*static_cast<int16_t*>(parameter) == -1) {
      result("Error", TFT_RED);
    } else {
      result("Success", TFT_GREEN);
    }
  });
  _readHandler = _dccExCS.addEventListener(DCCExCS::Event::PROGRAM_READ, [this](void* parameter) {
    if (*static_cast<int16_t*>(parameter) == -1) {
      result("Error", TFT_RED);
    } else {
      result(String(*static_cast<uint16_t*>(parameter)), TFT_GREEN);
    }
  });

  addElement<Header>(0, 40, 320, 18, "Program CV's");

  addElement<Button>(0, 70, 157, 40, "Read Address")
    ->onRelease([this](void*) {
      working();
      _dccExCS.getLocoAddress();
    });
  addElement<Button>(163, 70, 157, 40, "Write Address")
    ->onRelease([this](void*) {
      newStep(Step::WRITE_ADDRESS_GET_ADDRESS, "Enter Address", 10293, 1);
    });

  addElement<Button>(0, 117, 157, 40, "Read Byte")
    ->onRelease([this](void*) {
      newStep(Step::READ_CV_BYTE_GET_CV, "Enter CV Address", 1024, 1);
    });
  addElement<Button>(163, 117, 157, 40, "Write Byte")
    ->onRelease([this](void*) {
      newStep(Step::WRITE_CV_BYTE_GET_CV, "Enter CV Address", 1024, 1);
    });

  addElement<Button>(0, 164, 157, 40, "Read Bit")
    ->onRelease([this](void*) {
      newStep(Step::READ_CV_BIT_GET_CV, "Enter CV Address", 1024, 1);
    });
  addElement<Button>(163, 164, 157, 40, "Write Bit")
    ->onRelease([this](void*) {
      newStep(Step::WRITE_CV_BIT_GET_CV, "Enter CV Address", 1024, 1);
    });

  addElement<Header>(0, 218, 320, 18, "ACK settings");
  
  addElement<Button>(0, 248, 102, 40, "ACK Limit")
    ->onRelease([this](void*) {
      newStep(Step::ACK_LIMIT, "Enter ACK Limit", 100, 30);
    });

  addElement<Button>(109, 248, 102, 40, "ACK Min")
    ->onRelease([this](void*) {
      newStep(Step::ACK_MIN, "Enter ACK Min", 10000, 3000);
    });

  addElement<Button>(218, 248, 102, 40, "ACK Max")
    ->onRelease([this](void*) {
      newStep(Step::ACK_MAX, "Enter ACK Max", 10000, 3000);
    });
}

ProgramUI::~ProgramUI() {
  _dccExCS.removeEventListener(DCCExCS::Event::TIMEOUT, _timeoutHandler);
  _dccExCS.removeEventListener(DCCExCS::Event::PROGRAM_WRITE, _writeHandler);
  _dccExCS.removeEventListener(DCCExCS::Event::PROGRAM_READ, _readHandler);
}

void ProgramUI::newStep(Step step, const String& title, uint16_t max, uint16_t min) {
  _step = step;
  reset();
  auto keypad = setChild<Keypad>(title, max, min);
  keypad->addEventListener(Keypad::Event::ENTER, [this](void* parameter) {
    auto value = *static_cast<uint32_t*>(parameter);
    keypadEnter(value);
  });
  keypad->addEventListener(Keypad::Event::CANCEL, [this](void*) {
    reset(true);
  });
}

void ProgramUI::keypadEnter(uint32_t number) {
  switch (_step) {
    case Step::WRITE_ADDRESS_GET_ADDRESS: {
      String message = "Write Address?\nAddress: ";
      message += number;
      confirm(message, [this, number](void*) {
        working();
        _dccExCS.setLocoAddress(number);
      });
    } break;
    case Step::READ_CV_BYTE_GET_CV: {
      working();
      _dccExCS.getLocoCVByte(number);
    } break;
    case Step::WRITE_CV_BYTE_GET_CV: {
      _stepData[0] = number;
      newStep(Step::WRITE_CV_BYTE_GET_VALUE, "Enter Byte Value", 255, 0);
    } break;
    case Step::WRITE_CV_BYTE_GET_VALUE: {
      String message = "Write CV Value?\nCV: ";
      message += _stepData[0];
      message += "\nValue: ";
      message += number;
      confirm(message, [this, number](void*) {
        working();
        _dccExCS.setLocoCVByte(_stepData[0], number);
      });
    } break;
    case Step::READ_CV_BIT_GET_CV: {
      _stepData[0] = number;
      newStep(Step::READ_CV_BIT_GET_BIT, "Enter Bit", 7, 0);
    } break;
    case Step::READ_CV_BIT_GET_BIT: {
      working();
      _dccExCS.getLocoCVBit(_stepData[0], number);
    } break;
    case Step::WRITE_CV_BIT_GET_CV: {
      _stepData[0] = number;
      newStep(Step::WRITE_CV_BIT_GET_BIT, "Enter Bit", 7, 0);
    } break;
    case Step::WRITE_CV_BIT_GET_BIT: {
      _stepData[1] = number;
      newStep(Step::WRITE_CV_BIT_GET_VALUE, "Enter Value", 1, 0);
    } break;
    case Step::WRITE_CV_BIT_GET_VALUE: {
      String message = "Write CV Bit Value?\nCV: ";
      message += _stepData[0];
      message += "\nBit: ";
      message += _stepData[1];
      message += "\nValue: ";
      message += number;
      confirm(message, [this, number](void*) {
        working();
        _dccExCS.setLocoCVBit(_stepData[0], _stepData[1], number);
      });
    } break;
    case Step::ACK_LIMIT: {
      _dccExCS.setAckLimit(number);
      result("ACK Set", TFT_GREEN);
    } break;
    case Step::ACK_MIN: {
      _dccExCS.setAckMin(number);
      result("ACK Set", TFT_GREEN);
    } break;
    case Step::ACK_MAX: {
      _dccExCS.setAckMax(number);
      result("ACK Set", TFT_GREEN);
    } break;
  }
}

void ProgramUI::confirm(const String& message, Events::EventCallback&& callback) {
  reset();
  setChild<MessageBox>(message, std::vector<MessageBox::Button> {
    {
      "Yes",
      std::move(callback)
    },
    {
      "No",
      [this](void*) {
        reset(true);
      }
    }
  });
}

void ProgramUI::working() {
  reset();
  setChild<MessageBox>("Working...", TFT_BLUE);
}

void ProgramUI::result(const String& message, uint16_t color) {
  reset();
  setChild<MessageBox>(message, color, std::vector<MessageBox::Button> {
    {
      "Ok",
      [this](void*) {
        reset(true);
      }
    },
  });
}
