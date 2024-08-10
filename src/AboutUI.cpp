#include <AboutUI.h>
#include <Version.h>
#include <Elements/Header.h>

AboutUI::AboutUI(DCCExCS& dccExCS) : _dccExCS(dccExCS) {
  _elements.reserve(8);

  _csVersionHandler = _dccExCS.addEventListener(DCCExCS::Event::VERSION, [this](void* parameter) {
    auto version = *static_cast<DCCExCS::Version*>(parameter);

    _csVersion->setLabel("Version: " + version.version);
    _csBoard->setLabel("Board: " + version.board);
    _csShield->setLabel("Shield: " + version.shield);
    _csBuild->setLabel("Build: " + version.build);
  });

  addElement<Header>(0, 40, 320, 18, "About T3-EX-WiFi");

  String version("Version: ");
  version += T3_VERSION_MAJOR;
  version += ".";
  version += T3_VERSION_MINOR;
  version += ".";
  version += T3_VERSION_PATCH;
  addElement<Label>(0, 70, 320, 18, version, true);

  addElement<Label>(0, 97, 320, 18, "https://github.com/DCC-EX/EX-T3-WiFi", true);

  addElement<Header>(0, 125, 320, 18, "About DCC Ex");

  _csVersion = addElement<Label>(0, 155, 320, 18, "Not Detected", true);
  _csBoard = addElement<Label>(0, 182, 320, 18, "", true);
  _csShield = addElement<Label>(0, 209, 320, 18, "", true);
  _csBuild = addElement<Label>(0, 236, 320, 18, "", true);

  dccExCS.getCSVersion();
}

AboutUI::~AboutUI() {
  _dccExCS.removeEventListener(DCCExCS::Event::VERSION, _csVersionHandler);
}
