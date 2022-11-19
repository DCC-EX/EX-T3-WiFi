#include <ProgramUI.h>
#include <Elements/Button.h>

ProgramUI::ProgramUI(TFT_eSPI *tft, Tasks *tasks, DCCExCS *dccExCS) : UI(tft, tasks), _dccExCS(dccExCS) {
  _elements.reserve(10);

  _timeoutHandler = _dccExCS->addEventListener(static_cast<uint8_t>(DCCExCS::Event::TIMEOUT), [this](void *parameter) {
    result("Timeout", TFT_RED);
  });
  _writeHandler = _dccExCS->addEventListener(static_cast<uint8_t>(DCCExCS::Event::PROGRAM_WRITE), [this](void *parameter) {
    if (*static_cast<int16_t*>(parameter) == -1) {
      result("Error", TFT_RED);
    } else {
      result("Success", TFT_GREEN);
    }
  });
  _readHandler = _dccExCS->addEventListener(static_cast<uint8_t>(DCCExCS::Event::PROGRAM_READ), [this](void *parameter) {
    if (*static_cast<int16_t*>(parameter) == -1) {
      result("Error", TFT_RED);
    } else {
      char buf[7];
      sprintf(buf, "%d", *static_cast<uint16_t*>(parameter));
      result(buf, TFT_GREEN);
    }
  });

  buildUI();
}

ProgramUI::~ProgramUI() {
  _dccExCS->removeEventListener(static_cast<uint8_t>(DCCExCS::Event::TIMEOUT), _timeoutHandler);
  _dccExCS->removeEventListener(static_cast<uint8_t>(DCCExCS::Event::PROGRAM_WRITE), _writeHandler);
  _dccExCS->removeEventListener(static_cast<uint8_t>(DCCExCS::Event::PROGRAM_READ), _readHandler);
}

void ProgramUI::rotated() {
  if (_keypad != nullptr) {
    _keypad->rotated();
  } else {
    UI::rotated();
  }
}

void ProgramUI::touch(uint8_t count, GTPoint* points) {
  if (_keypad != nullptr) {
    _keypad->touch(count, points);
  } else {
    UI::touch(count, points);
  }
}

void ProgramUI::release(uint8_t count, GTPoint* points) {
  if (_keypad != nullptr) {
    _keypad->release(count, points);
  } else {
    UI::release(count, points);
  }
}

void ProgramUI::buildUI() {
  addElement<Header>(0, 40, 320, 18, "Program CV's");

  addElement<Button>(0, 70, 320, 40, "Write Address")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::WRITE_ADDRESS_GET_ADDRESS, "Enter Address", 10293, 1));

  addElement<Button>(0, 117, 320, 40, "Write Byte")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::WRITE_CV_BYTE_GET_CV, "Enter CV Address", 1024, 1));

  addElement<Button>(0, 164, 320, 40, "Write Bit")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::WRITE_CV_BIT_GET_CV, "Enter CV Address", 1024, 1));

  addElement<Button>(0, 211, 320, 40, "Read Address")
    ->onRelease([this](void *parameter) {
      _tasks->push_back([this]() {
        working();
        _dccExCS->readLocoAddress();
      });
    });

  addElement<Button>(0, 258, 320, 40, "Read Byte")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::READ_CV_BYTE_GET_CV, "Enter CV Address", 1024, 1));

  addElement<Header>(0, 312, 320, 18, "ACK settings");
  
  addElement<Button>(0, 342, 320, 40, "ACK Limit")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::ACK_LIMIT, "Enter ACK Limit", 100, 30));

  addElement<Button>(0, 389, 320, 40, "ACK Min")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::ACK_MIN, "Enter ACK Min", 10000, 3000));

  addElement<Button>(0, 436, 320, 40, "ACK Max")
    ->onRelease(std::bind(&ProgramUI::newStep, this, Step::ACK_MAX, "Enter ACK Max", 10000, 3000));
}

void ProgramUI::clearUI() {
  _tft->fillRect(0, 30, 320, 450, TFT_BLACK);

  _elements.clear();
  _keypad.reset();
}

void ProgramUI::newStep(Step step, const char *title, uint16_t max, uint16_t min) {
  _tasks->push_back([this, step, title, max, min]() {
    clearUI();
    _step = step;
    _keypad = std::make_unique<Keypad>(_tft, _tasks, title, max, min);
    _keypad->addEventListener(static_cast<uint8_t>(Keypad::Event::ENTER), [this](void *parameter) {
      _tasks->push_back(std::bind(&ProgramUI::keyPadEnter, this));
    });
    _keypad->addEventListener(static_cast<uint8_t>(Keypad::Event::CANCEL), [this](void *parameter) {
      _tasks->push_back([this]() {
        clearUI();
        buildUI();
      });
    });
  });
}

void ProgramUI::keyPadEnter() {
  uint32_t number = _keypad->getNumber();
  switch (_step) {
    case Step::WRITE_ADDRESS_GET_ADDRESS: {
      working();
      _dccExCS->writeLocoAddress(number);
    } break;
    case Step::WRITE_CV_BYTE_GET_CV: {
      _stepData[0] = number;
      newStep(Step::WRITE_CV_BYTE_GET_VALUE, "Enter Byte Value", 255, 0);
    } break;
    case Step::WRITE_CV_BYTE_GET_VALUE: {
      working();
      _dccExCS->writeLocoCVByte(_stepData[0], number);
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
      working();
      _dccExCS->writeLocoCVBit(_stepData[0], _stepData[1], number);
    } break;
    case Step::READ_CV_BYTE_GET_CV: {
      working();
      _dccExCS->readLocoCVByte(number);
    } break;
    case Step::ACK_LIMIT: {
      _dccExCS->setAckLimit(number);
      result("ACK Set", TFT_GREEN);
    } break;
    case Step::ACK_MIN: {
      _dccExCS->setAckMin(number);
      result("ACK Set", TFT_GREEN);
    } break;
    case Step::ACK_MAX: {
      _dccExCS->setAckMax(number);
      result("ACK Set", TFT_GREEN);
    } break;
  }
}

void ProgramUI::working() {
  clearUI();
  addElement<Label>(0, 220, 320, 40, "Working...", true, TFT_BLUE);
}

void ProgramUI::result(const char *label, uint16_t color) {
  clearUI();
  addElement<Label>(0, 200, 320, 40, label, true, color);
  addElement<Button>(0, 250, 320, 40, "Ok")
    ->onRelease([this](void *parameter) {
      _tasks->push_back([this]() {
        clearUI();
        buildUI();
      });
    });
}
