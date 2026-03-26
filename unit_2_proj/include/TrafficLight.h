#ifndef TRAFFIC_LIGH_H
#define TRAFFIC_LIGH_H

#include "ITrafficLight.h"

class TrafficLight : public ITrafficLight {
  public:
    TrafficLight(ITrafficLight& normal, ITrafficLight& manitained);

    void start() override;
    void stop() override;
    void onTick() override;

    void toggleMode();

  private:
    ITrafficLight& _normal;
    ITrafficLight& _maintaned;
    ITrafficLight* _current;
};

#endif
