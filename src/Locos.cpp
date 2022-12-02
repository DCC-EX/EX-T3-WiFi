#include <Locos.h>

Locos::Locos() {
  _locos.reserve(50);
}

void Locos::change() {
  uint8_t count = _locos.size();
  dispatchEvent(Event::COUNT_CHANGE, &count);
}

Locos::operator uint16_t() {
  return _locos.size() ? _locos[_active] : 0;
};

void Locos::add(uint16_t address) {
  if (_locos.size() > 0 && _locos[_active] == address) {
    return;
  }

  auto it = std::find(_locos.begin(), _locos.end(), address);
  if (it != _locos.end()) {
    _active = it - _locos.begin();
    return;
  }

  _locos.push_back(address);
  _active = _locos.size() - 1;

  change();
}

uint16_t Locos::remove() {
  uint16_t address = *this;

  auto it = std::find(_locos.begin(), _locos.end(), address);
  if (it != _locos.end()) {
    _active = std::min<uint16_t>(_locos.erase(it) - _locos.begin(), _locos.size() - 1);
  }

  change();

  return address;
}

void Locos::next() {
  _active++;
  if (_active >= _locos.size()) {
    _active = 0;
  }
}

void Locos::prev() {
  if (_active == 0) {
    _active = _locos.size() - 1;
  } else {
    _active--;
  }
}
