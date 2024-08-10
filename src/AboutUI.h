#ifndef ABOUT_UI_H
#define ABOUT_UI_H

#include <UI.h>
#include <DCCExCS.h>
#include <Elements/Label.h>

class AboutUI : public UI {
  private:
    DCCExCS& _dccExCS;
    uint16_t _csVersionHandler;

    Label* _csVersion;
    Label* _csBoard;
    Label* _csShield;
    Label* _csBuild;
  public:
    AboutUI(DCCExCS& dccExCS);
    ~AboutUI();
};

#endif
